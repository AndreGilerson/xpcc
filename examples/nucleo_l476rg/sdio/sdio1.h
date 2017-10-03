#ifndef SDIO1_H
#define SDIO1_H

#include <xpcc/architecture/interface/spi_master.hpp>
#include "stm32l476xx.h"

class Sdio1
{
public:
	enum class
	BusWidth : uint32_t
	{
		BusWidth1B	= 0x00000000,
		BusWidth	= SDMMC_CLKCR_WIDBUS_1
	};

	enum class
	PowerSave : uint32_t
	{
		Enable	= 0x00000000,
		Disable	= SDMMC_CLKCR_PWRSAV
	};

	enum class
	ClockBypass : uint32_t
	{
		Disable	= 0x00000000,
		Enable	= SDMMC_CLKCR_BYPASS
	};

	static const uint32_t SD_CMD_CLEAR_MSK = SDMMC_CMD_CMDINDEX | SDMMC_CMD_WAITRESP |
										SDMMC_CMD_WAITINT | SDMMC_CMD_WAITPEND |
										SDMMC_CMD_CPSMEN | SDMMC_CMD_SDIOSUSPEND;

	enum class
	Command : uint32_t
	{
		SD_GO_IDLE_STATE		= 0,	//Resets SD card
		SD_GET_CID				= 2,
		SD_SEND_REL_ADD			= 3,
		SD_SELECT_CARD			= 7,
		SD_HS_SEND_EXT_CSD		= 8,
		SD_SEND_CSD				= 9,

		SD_STOP_TRANSMISSION	= 12,

		SD_SET_BLOCK_SIZE		= 16,
		SD_READ_SINGLE_BLOCK	= 17,
		SD_READ_MULTI_BLOCK		= 18,

		SD_WRITE_SINGLE_BLOCK	= 24,
		SD_WRITE_MULTI_BLOCK	= 25,

		SD_APP_CMD				= 55,
		SD_READ_OCR				= 58,

		SD_SD_APP_OP_COND		= 41,
		SD_SEND_STATUS			= 13
	};

	static const uint32_t SD_CHECK_VOLTAGE	= 0x1AA;
	static const uint32_t SD_MAX_TRIAL		= 0xFFFF;
	static const uint32_t SD_RESP1_NO_ERROR	= 0xFDFFE008;
	static const uint32_t SD_VOLTAGE_WINDOW	= 0x80100000;
	static const uint32_t SD_HIGH_CAPACITY	= 0x40000000;

	enum class
	CommandResponse : uint32_t
	{
		None	= 0x00000000,
		Short	= SDMMC_CMD_WAITRESP_0,
		Long	= SDMMC_CMD_WAITRESP
	};

	enum class
	CommandError : uint8_t
	{
		Success	= 0,
		Timeout	= 1,
		CrcFail	= 2
	};

	enum class
	CardType : uint32_t
	{
		StdCapacityV1	= 0,
		HighCapacity,
		MMC
	};

protected:
	static CardType cardType;
	static uint32_t relativeAddress;
	static uint32_t deviceSize;
	static uint32_t maxTransferRate;

public:
	Sdio1()
	{

	}

	template< class SystemClock, uint32_t baudrate,
					uint16_t tolerance = xpcc::Tolerance::FivePercent >
	static void
	initialize()
	{
		RCC->CCIPR |= RCC_CCIPR_CLK48SEL_0 | RCC_CCIPR_CLK48SEL_1;

		RCC->APB2ENR |= RCC_APB2ENR_SDMMC1EN;
		RCC->APB2RSTR |= RCC_APB2RSTR_SDMMC1RST;
		RCC->APB2RSTR &= ~RCC_APB2RSTR_SDMMC1RST;

		SDMMC1->POWER = SDMMC_POWER_PWRCTRL_1 | SDMMC_POWER_PWRCTRL_0;
		xpcc::delayMilliseconds(1);
		SDMMC1->CLKCR = SDMMC_CLKCR_CLKEN | 0x76;  //During Initialization clk frequnecy has to be lower than 400Khz
	}

	static void
	powerOn()
	{
		RCC->CCIPR |= RCC_CCIPR_CLK48SEL_0 | RCC_CCIPR_CLK48SEL_1;

		RCC->APB2ENR |= RCC_APB2ENR_SDMMC1EN;
		RCC->APB2RSTR |= RCC_APB2RSTR_SDMMC1RST;
		RCC->APB2RSTR &= ~RCC_APB2RSTR_SDMMC1RST;

		SDMMC1->POWER = SDMMC_POWER_PWRCTRL_1 | SDMMC_POWER_PWRCTRL_0;
		xpcc::delayMilliseconds(1);
	}

	static void
	powerOff()
	{

	}

	static uint8_t
	initializeCard()
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
			return 1;
		if(sendCommand(Command::SD_SEND_REL_ADD, CommandResponse::Short, 0) != CommandError::Success)
			return 1;
		relativeAddress = getCommandResp1();
		getCardInfo();
		if(sendCommand(Command::SD_SELECT_CARD, CommandResponse::Short, relativeAddress) != CommandError::Success)
			return 1;

		return 0;
	}

	static uint8_t
	readBlocks(void* data, uint32_t numberOfBlocks, uint32_t blockAdress)
	{
		if(cardType == CardType::HighCapacity)
		{
			blockAdress *= 512;
		}

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
		}

		SDMMC1->ICR = 0Xffffffff;
		if(sendCommand(Command::SD_STOP_TRANSMISSION, CommandResponse::Short, 0) != CommandError::Success)
			return 1;
	}

	static uint8_t
	writeBlocks(void* data, uint32_t numberOfBlocks, uint32_t blockAddress)
	{
		if(cardType == CardType::HighCapacity)
		{
			blockAddress *= 512;
		}

		SDMMC1->DCTRL = 0;
		SDMMC1->DTIMER = 0xffffffff;
		SDMMC1->DLEN = numberOfBlocks*512;
		SDMMC1->DCTRL = SDMMC_DCTRL_DBLOCKSIZE_0 | SDMMC_DCTRL_DBLOCKSIZE_3 | SDMMC_DCTRL_DTEN;

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
		}

		SDMMC1->ICR = 0Xffffffff;
		if(sendCommand(Command::SD_STOP_TRANSMISSION, CommandResponse::Short, 0) != CommandError::Success)
			return 1;
	}

	static uint8_t
	getCardInfo()
	{
		if(sendCommand(Command::SD_SEND_CSD, CommandResponse::Long, relativeAddress) != CommandError::Success)
			return 1;

		uint32_t csd[4];
		csd[0] = SDMMC1->RESP1;
		csd[1] = SDMMC1->RESP2;
		csd[2] = SDMMC1->RESP3;
		csd[3] = SDMMC1->RESP4;

		deviceSize = (csd[1] & 0x0000003F) << 16;
		deviceSize = (csd[2] & 0xFFFF0000) >> 16;
		deviceSize *= 512 * 1024;

		uint8_t tmp = (csd[3] & 0xFF000000) >> 24;
		uint32_t unit = 0;
		switch(tmp & 0xE0)
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

protected:

	static CommandError
	sendCommand(Command cmd, CommandResponse resp, uint32_t argument)
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

	static uint32_t
	getCommandResp1()
	{
		return SDMMC1->RESP1 & SD_RESP1_NO_ERROR;
	}


};

Sdio1::CardType Sdio1::cardType(Sdio1::CardType::StdCapacityV1);
uint32_t Sdio1::relativeAddress(0);
uint32_t Sdio1::deviceSize(0);
uint32_t Sdio1::maxTransferRate(0);

#endif // SDIO1_H
