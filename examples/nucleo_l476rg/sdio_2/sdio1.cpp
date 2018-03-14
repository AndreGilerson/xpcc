#include "sdio1.h"

#include <fatfs/diskio.h>
#include <fatfs/ff.h>

Sdio1 sdio;

Sdio1::Sdio1()
{
}

void Sdio1::powerOn()
{
	RCC->CCIPR |= RCC_CCIPR_CLK48SEL_0 | RCC_CCIPR_CLK48SEL_1;

	RCC->APB2ENR |= RCC_APB2ENR_SDMMC1EN;
	RCC->APB2RSTR |= RCC_APB2RSTR_SDMMC1RST;
	RCC->APB2RSTR &= ~RCC_APB2RSTR_SDMMC1RST;

	SDMMC1->POWER = SDMMC_POWER_PWRCTRL_1 | SDMMC_POWER_PWRCTRL_0;
	xpcc::delayMilliseconds(1);

	//The following needs to be a configurable parameter via scons, here until initial version is complete
	//DMA can be activated for asynchronous data transfer
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
	SDMMC1->MASK = 0;
	xpcc::delayMilliseconds(1);
}

void Sdio1::powerOff()
{

}

uint8_t Sdio1:: initializeCard()
{
	SDMMC1->CLKCR |= SDMMC_CLKCR_CLKEN | 0x76;  //During Initialization clk frequnecy has to be lower than 400Khz

	if(sendCommand(Command::SD_GO_IDLE_STATE, CommandResponse::None, 0) != CommandError::Success)
		return 1;

	if(sendCommand(Command::SD_HS_SEND_EXT_CSD, CommandResponse::Short, SD_CHECK_VOLTAGE) == CommandError::Success)
	{
		cardType = CardType::HighCapacity;
	}

	if(sendCommand(Command::SD_APP_CMD, CommandResponse::Short, 0) == CommandError::Success)
	{
		uint32_t voltage = 0;
		uint32_t tries = 0;
		while(!voltage && tries < SD_MAX_TRIAL)
		{
			if(sendCommand(Command::SD_APP_CMD, CommandResponse::Short, 0) != CommandError::Success)
				return 1;

			if(sendCommand(Command::SD_SD_APP_OP_COND, CommandResponse::Short,
						   SD_VOLTAGE_WINDOW | SD_HIGH_CAPACITY) != CommandError::CrcFail)
				return 1;
			uint32_t resp = getCommandResp1();
			voltage = (((resp >> 31) == 1) ? 1 : 0);
			tries++;
		}

		if(tries >= SD_MAX_TRIAL)
			return 1;
	}
	else
	{
		cardType = CardType::MMC;
	}

	if(sendCommand(Command::SD_GET_CID, CommandResponse::Long, 0) != CommandError::Success)
	{
		return 1;
	}
	if(sendCommand(Command::SD_SEND_REL_ADD, CommandResponse::Short, 0) != CommandError::Success)
	{
		return 1;
	}
	relativeAddress = getCommandResp1();
	getCardInfo();
	if(sendCommand(Command::SD_SELECT_CARD, CommandResponse::Short, relativeAddress) != CommandError::Success)
	{
		return 1;
	}

	//The following needs to be a configurable parameter via scons, here until initial version is complete
	//SD cards can either operate with 1 or 4 data channels
	if(sendCommand(Command::SD_APP_CMD, CommandResponse::Short, relativeAddress) != CommandError::Success)
		return 1;
	if(sendCommand(Command::SD_SET_BUS_WIDTH, CommandResponse::Short, 0x02) != CommandError::Success)
		return 1;
	uint32_t temp = SDMMC1->CLKCR | SDMMC_CLKCR_WIDBUS_0;
	temp &= ~SDMMC_CLKCR_CLKDIV_Msk;
	temp |= 0x8;
	SDMMC1->CLKCR = temp;

	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

	return 0;
}

uint8_t Sdio1::readBlocks(const void* data, uint32_t numberOfBlocks, uint32_t blockAdress)
{
	NVIC_DisableIRQ(SDMMC1_IRQn);

	SDMMC1->DCTRL = 0;
	SDMMC1->DTIMER = 0xffffffff;
	SDMMC1->DLEN = numberOfBlocks*512;
	SDMMC1->DCTRL = SDMMC_DCTRL_DBLOCKSIZE_0 | SDMMC_DCTRL_DBLOCKSIZE_3 | SDMMC_DCTRL_DTDIR | SDMMC_DCTRL_DTEN;

	CommandError error = CommandError::Success;
	if(numberOfBlocks == 1)
	{
		error = sendCommand(Command::SD_READ_SINGLE_BLOCK, CommandResponse::Short, blockAdress);
	} else
	{
		error = sendCommand(Command::SD_READ_MULTI_BLOCK, CommandResponse::Short, blockAdress);
	}

	uint32_t* tempbuff = (uint32_t*) data;
	if(error == CommandError::Success)
	{
		while(!(SDMMC1->STA & (SDMMC_STA_RXOVERR | SDMMC_STA_DCRCFAIL | SDMMC_STA_DTIMEOUT | SDMMC_STA_DATAEND)))
		{
			if(SDMMC1->STA & SDMMC_STA_RXFIFOHF)
			{
				for(uint32_t count = 0U; count < 8U; count++)
				{
					*(tempbuff + count) = SDMMC1->FIFO;
				}
				tempbuff += 8U;
			}
		}
	} else
	{
		return 1;
	}

	SDMMC1->ICR = 0Xffffffff;
	if(numberOfBlocks > 1)
	{
		if(sendCommand(Command::SD_STOP_TRANSMISSION, CommandResponse::Short, 0) != CommandError::Success)
		{
			return 1;
		}
	}
	return 0;
}

uint8_t Sdio1::readBlocksDMA(const void* data, uint32_t numberOfBlocks, uint32_t blockAdress)
{
	CommandError error = CommandError::Success;

	transferComplete = false;
	readStarted = true;

	uint32_t statusResponse = 0;
	do
	{
		error = sendCommand(Command::SD_SEND_STATUS, CommandResponse::Short, relativeAddress);
		statusResponse = (SDMMC1->RESP1 >> 9) & 0x0000000F; //TODO Change getCommandResp1() function, to return all values.
	} while(statusResponse == 7 || statusResponse == 6);

	NVIC_EnableIRQ(SDMMC1_IRQn);
	SDMMC1->MASK = SDMMC_MASK_DCRCFAILIE | SDMMC_MASK_DTIMEOUTIE | SDMMC_MASK_DATAENDIE | SDMMC_MASK_RXOVERRIE;


	DMA2_CSELR->CSELR = 0x7 << 12;

	DMA2_Channel4->CCR = 0;
	DMA2->IFCR = 0; //DMA_IFCR_CTCIF4 | DMA_IFCR_CTEIF4 | DMA_IFCR_CHTIF4 | DMA_IFCR_CGIF4;

	DMA2_Channel4->CPAR = 0x40012880;
	DMA2_Channel4->CMAR = (uint32_t) data;
	DMA2_Channel4->CNDTR = (uint32_t) 512*numberOfBlocks/4;

	DMA2_Channel4->CCR = DMA_CCR_PL_1 | DMA_CCR_PL_0 | DMA_CCR_MSIZE_1 | DMA_CCR_PSIZE_1 | DMA_CCR_MINC |
			DMA_CCR_EN; // | DMA_CCR_TCIE | DMA_CCR_TEIE;

	SDMMC1->DCTRL = 0;
	SDMMC1->DTIMER = 0xffffffff;
	SDMMC1->DLEN = numberOfBlocks*512;
	SDMMC1->DCTRL = SDMMC_DCTRL_DBLOCKSIZE_0 | SDMMC_DCTRL_DBLOCKSIZE_3 | SDMMC_DCTRL_DTDIR | SDMMC_DCTRL_DTEN | SDMMC_DCTRL_DMAEN;

	if(numberOfBlocks == 1)
	{
		error = sendCommand(Command::SD_READ_SINGLE_BLOCK, CommandResponse::Short, blockAdress);
	} else
	{
		error = sendCommand(Command::SD_READ_MULTI_BLOCK, CommandResponse::Short, blockAdress);
	}
}

uint8_t Sdio1::writeBlocks(const void* data, uint32_t numberOfBlocks, uint32_t blockAddress)
{
	NVIC_DisableIRQ(SDMMC1_IRQn);

	SDMMC1->DCTRL = 0;
	SDMMC1->DTIMER = 0xffffffff;
	SDMMC1->DLEN = numberOfBlocks*512;
	SDMMC1->DCTRL = SDMMC_DCTRL_DBLOCKSIZE_0 | SDMMC_DCTRL_DBLOCKSIZE_3 | SDMMC_DCTRL_DTEN;
	SDMMC1->ICR = SDMMC_ICR_DTIMEOUTC;

	CommandError error = CommandError::Success;
	if(numberOfBlocks == 1)
	{
		error = sendCommand(Command::SD_WRITE_SINGLE_BLOCK, CommandResponse::Short, blockAddress);
	} else
	{
		error = sendCommand(Command::SD_WRITE_MULTI_BLOCK, CommandResponse::Short, blockAddress);
	}

	uint32_t* tempbuff = (uint32_t*) data;
	if(error == CommandError::Success)
	{
		while(!(SDMMC1->STA & (SDMMC_STA_TXUNDERR | SDMMC_STA_DCRCFAIL | SDMMC_STA_DTIMEOUT | SDMMC_STA_DATAEND)))
		{
			if(SDMMC1->STA & SDMMC_STA_TXFIFOHE)
			{
				for(uint32_t count = 0U; count < 8U; count++)
				{
					SDMMC1->FIFO = *(tempbuff + count);
				}
				tempbuff += 8U;
			}
		}
	} else
	{
		return 1;
	}

	while(SDMMC1->STA & SDMMC_STA_TXACT || SDMMC1->STA & SDMMC_STA_TXDAVL);

	SDMMC1->ICR = 0Xffffffff;
	if(numberOfBlocks > 1)
	{
		if(sendCommand(Command::SD_STOP_TRANSMISSION, CommandResponse::Short, 0) != CommandError::Success)
			return 1;
	}

	uint32_t statusResponse = 0;
	do
	{
		error = sendCommand(Command::SD_SEND_STATUS, CommandResponse::Short, relativeAddress);
		statusResponse = (SDMMC1->RESP1 >> 9) & 0x0000000F; //TODO Change getCommandResp1() function, to return all values.
	} while(statusResponse == 7 || statusResponse == 6);
	return 0;
}

uint8_t Sdio1::writeBlocksDMA(const void* data, uint32_t numberOfBlocks, uint32_t blockAddress)
{
	CommandError error = CommandError::Success;

	transferComplete = false;
	writeStarted = true;

	NVIC_EnableIRQ(SDMMC1_IRQn);
	SDMMC1->MASK = SDMMC_MASK_DCRCFAILIE | SDMMC_MASK_DTIMEOUTIE | SDMMC_MASK_DATAENDIE | SDMMC_MASK_RXOVERRIE;

	DMA2_CSELR->CSELR = 0x7 << 16;

	DMA2_Channel5->CCR = 0;
	DMA2->IFCR = 0;

	DMA2_Channel5->CPAR = 0x40012880;
	DMA2_Channel5->CMAR = (uint32_t) data;
	DMA2_Channel5->CNDTR = (uint32_t) numberOfBlocks*512/4;

	DMA2_Channel5->CCR = DMA_CCR_PL_1 | DMA_CCR_PL_0 | DMA_CCR_MSIZE_1 | DMA_CCR_PSIZE_1 | DMA_CCR_MINC | DMA_CCR_DIR |
			DMA_CCR_EN; // | DMA_CCR_TCIE | DMA_CCR_TEIE;

	if(numberOfBlocks == 1)
	{
		error = sendCommand(Command::SD_WRITE_SINGLE_BLOCK, CommandResponse::Short, blockAddress);
	} else
	{
		error = sendCommand(Command::SD_WRITE_MULTI_BLOCK, CommandResponse::Short, blockAddress);
	}

	SDMMC1->DCTRL = 0;
	SDMMC1->DTIMER = 0xffffffff;
	SDMMC1->DLEN = numberOfBlocks*512;
	SDMMC1->DCTRL = SDMMC_DCTRL_DBLOCKSIZE_0 | SDMMC_DCTRL_DBLOCKSIZE_3 | SDMMC_DCTRL_DTEN | SDMMC_DCTRL_DMAEN;
}

uint8_t Sdio1::getCardInfo()
{
	if(sendCommand(Command::SD_SEND_CSD, CommandResponse::Long, relativeAddress) != CommandError::Success)
		return 1;

	uint32_t csd[4];
	csd[0] = SDMMC1->RESP1;
	csd[1] = SDMMC1->RESP2;
	csd[2] = SDMMC1->RESP3;
	csd[3] = SDMMC1->RESP4;

	deviceSize = 0;
	deviceSize |= (csd[1] & 0x0000003F) << 16;
	deviceSize |= (csd[2] & 0xFFFF0000) >> 16;
	sectorCount = (deviceSize+1) * 1024;
	deviceSize =(deviceSize+1) * 512*1024;

	uint8_t tmp = (csd[3] & 0xFF000000) >> 24;
	uint32_t unit = 0;
	switch((tmp & 0xE0) >> 5)
	{
	case 0:
		unit = 100;
		break;
	case 1:
		unit = 1000;
		break;
	case 2:
		unit = 10000;
		break;
	case 3:
		unit = 100000;
		break;
	}

	switch(tmp & 0x1E)
	{
	case 0x1:
		maxTransferRate = unit;
		break;
	case 0x2:
		maxTransferRate = 1.2 * unit;
		break;
	case 0x3:
		maxTransferRate = 1.3 * unit;
		break;
	case 0x4:
		maxTransferRate = 1.5 * unit;
		break;
	case 0x5:
		maxTransferRate = 2 * unit;
		break;
	case 0x6:
		maxTransferRate = 2.5 * unit;
		break;
	case 0x7:
		maxTransferRate = 3.0 * unit;
		break;
	case 0x8:
		maxTransferRate = 3.5 * unit;
		break;
	case 0x9:
		maxTransferRate = 4.0 * unit;
		break;
	case 0xA:
		maxTransferRate = 4.5 * unit;
		break;
	case 0xB:
		maxTransferRate = 5.0 * unit;
		break;
	case 0xC:
		maxTransferRate = 5.5 * unit;
		break;
	case 0xD:
		maxTransferRate = 6.0 * unit;
		break;
	case 0xE:
		maxTransferRate = 7.0 * unit;
		break;
	case 0xF:
		maxTransferRate = 8.0 * unit;
		break;
	}
}

bool Sdio1::isTransferComplete()
{
	return transferComplete;
}

Sdio1::CommandError Sdio1::sendCommand(Command cmd, CommandResponse resp, uint32_t argument)
{
	SDMMC1->ICR = SDMMC_ICR_CCRCFAILC | SDMMC_ICR_CTIMEOUTC | SDMMC_ICR_CMDRENDC | SDMMC_ICR_CMDSENTC;

	SDMMC1->ARG = argument;
	SDMMC1->CMD = ~SD_CMD_CLEAR_MSK | static_cast<uint32_t>(cmd) | static_cast<uint32_t>(resp) | SDMMC_CMD_CPSMEN;
	if(resp == CommandResponse::None)
		while(!(SDMMC1->STA & (SDMMC_STA_CTIMEOUT | SDMMC_STA_CMDSENT)));
	else
		while(!(SDMMC1->STA & (SDMMC_STA_CTIMEOUT | SDMMC_STA_CMDREND | SDMMC_STA_CCRCFAIL)));

	if(SDMMC1->STA & SDMMC_STA_CTIMEOUT) {
		return CommandError::Timeout;
	}
	if(SDMMC1->STA & SDMMC_STA_CCRCFAIL) {
		return CommandError::CrcFail;
	}
	return CommandError::Success;
}

uint32_t Sdio1::getCommandResp1()
{
	return SDMMC1->RESP1 & SD_RESP1_NO_ERROR;
}

uint32_t Sdio1::getSectorCount()
{
	return sectorCount;
}

Sdio1::CardType Sdio1::cardType(Sdio1::CardType::StdCapacityV1);
uint32_t Sdio1::relativeAddress(0);
uint64_t Sdio1::deviceSize(0);
uint32_t Sdio1::sectorCount(0);
uint32_t Sdio1::maxTransferRate(0);
bool Sdio1::transferComplete(false);
bool Sdio1::writeStarted(false);
bool Sdio1::readStarted(false);

bool initialized = 0;

extern "C"
{
	DWORD get_fattime(void)
	{
		return (((2010L - 1980L) << 25) |
				(5L << 21) |
				(30L << 16) |
				(16L << 11) |
				(0 << 5) |
				(0));
	}


	DSTATUS disk_status (BYTE pdrv)
	{
		if(initialized)
		{
			return 0;
		} else
		{
			return STA_NOINIT;
		}
	}

	DSTATUS disk_initialize (BYTE pdrv)
	{
		if(!initialized)
		{
			sdio.powerOn();
			if(sdio.initializeCard())
			{
				return 1;
			}
			initialized = 1;
		}
		return 0;
	}

	DRESULT disk_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
	{
		if(!sdio.readBlocks(buff, count, sector))
		{
			return RES_OK;
		}
		return RES_ERROR;
	}

	DRESULT disk_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
	{
		if(!sdio.writeBlocks(buff, count, sector))
		{
			return RES_OK;
		}
		return RES_ERROR;
	}

	DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void *buff)
	{
		sdio.getCardInfo();
		if(cmd == GET_SECTOR_COUNT)
		{
			*((uint32_t*) buff) = sdio.getSectorCount();
		} else if (cmd == GET_BLOCK_SIZE)
		{
			*((uint32_t*) buff) = 512;
		}
		return RES_OK;
	}
}

//The following needs to be a configurable parameter via scons, here until intial version is complete
//SDMMC can be used in either blocking/polling mode or asynch with DMA. In the latter case Interrupt needs to be enabled
XPCC_ISR(SDMMC1, xpcc_fastcode)
{
	if((SDMMC1->STA & SDMMC_STA_DATAEND) != 0)
	{
		Sdio1::transferComplete = true;
		SDMMC1->ICR = SDMMC_ICR_DATAENDC;
		Sdio1::sendCommand(Sdio1::Command::SD_STOP_TRANSMISSION, Sdio1::CommandResponse::Short, 0);

		if(Sdio1::readStarted)
		{
			DMA2->IFCR = DMA_IFCR_CTCIF4 | DMA_IFCR_CTEIF4 | DMA_IFCR_CHTIF4 | DMA_IFCR_CGIF4;
			DMA2_Channel4->CCR &= ~DMA_CCR_EN;
			Sdio1::readStarted = 0;
		}

		if(Sdio1::writeStarted)
		{
			DMA2->IFCR = DMA_IFCR_CTCIF5 | DMA_IFCR_CTEIF5 | DMA_IFCR_CHTIF5 | DMA_IFCR_CGIF5;
			DMA2_Channel5->CCR &= ~DMA_CCR_EN;
			Sdio1::writeStarted = 0;
		}
	}
}

XPCC_ISR(DMA2_Channel4, xpcc_fastcode)
{
	DMA2->IFCR = DMA_IFCR_CTCIF4 | DMA_IFCR_CTEIF4 | DMA_IFCR_CHTIF4 | DMA_IFCR_CGIF4;
	while(SDMMC1->FIFOCNT);
	DMA2_Channel4->CCR &= ~DMA_CCR_EN;
	SDMMC1->DCTRL = 0;
}


XPCC_ISR(DMA2_Channel5, xpcc_fastcode)
{
	DMA2->IFCR = DMA_IFCR_CTCIF5 | DMA_IFCR_CTEIF5 | DMA_IFCR_CHTIF5 | DMA_IFCR_CGIF5;
	while(SDMMC1->FIFOCNT);
	DMA2_Channel5->CCR &= ~DMA_CCR_EN;
	SDMMC1->DCTRL = 0;
}
