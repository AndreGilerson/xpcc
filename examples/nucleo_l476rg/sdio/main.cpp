#include <xpcc/architecture/platform.hpp>
#include <xpcc/architecture/interface/gpio.hpp>

#include <stm32l4xx.h>
#include "sdio1.h"

#include <fatfs/diskio.h>
#include <fatfs/ff.h>

using Data0	= xpcc::stm32::GpioC8;
using Data1 = xpcc::stm32::GpioC9;
using Data2 = xpcc::stm32::GpioC10;
using Data3 = xpcc::stm32::GpioC11;
using Sck	= xpcc::stm32::GpioC12;
using Cmd	= xpcc::stm32::GpioD2;

using Sdio = xpcc::stm32::SdioBase;

uint32_t x32 = 314159265;
uint32_t xorshift32()
{
	x32 ^= x32 << 13;
	x32 ^= x32 >> 17;
	x32 ^= x32 << 5;
	return x32;
}

extern Sdio1 sdio;
extern bool initialized;

using Tx = GpioOutputA2;
using Rx = GpioInputA3;
using Uart = Usart2;

uint8_t UIntToString(uint32_t in, char* out)
{
	uint8_t i = 0;
	uint8_t j = 0;
	char temp[32] = {0};
	uint8_t numberOfDigits = 0;

	do
	{
		temp[i] = (char) (in % 10) + '0'; //Digits are calculated by adding the remainder of division by 10 to the ASCII code of '0'
		in /= 10;
		i++;
	} while(in);

	numberOfDigits = i;
	out[i] = '\0';

	while(i) //The String has to be reversed to be in correct order
	{
		out[i-1] = temp[j];
		i--;
		j++;
	}

	return numberOfDigits;
}

int main()
{
	static char testBuffer[10240];
	static char writeBuffer[10240];

	Board::initialize();

	Data0::connect(Sdio::Data0);
	Data1::connect(Sdio::Data1);
	Data2::connect(Sdio::Data2);
	Data3::connect(Sdio::Data3);
	Sck::setOutput();
	Sck::connect(Sdio::Sck, Gpio::OutputType::PushPull, Gpio::OutputSpeed::MHz50);
	Cmd::connect(Sdio::Cmd);

	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<Board::systemClock, 115200>(12);

	//PHASE 1 TESTING:---------------------------------------
	//-------------------------------------------------------

	Uart::writeBlocking((uint8_t*) "Start Phase1 testing\n\r", 22);

	for(int i = 0; i < 10240; i++)
	{
		testBuffer[i] = 0;
	}

	for(int i = 0; i < 10240; i++)
	{
		writeBuffer[i] = xorshift32();
	}


	sdio.powerOn();
	if(sdio.initializeCard())
	{
		Uart::writeBlocking((uint8_t*) "Error initializing card. Stopped\n\r", 34);
		while(1);
	}

	initialized = 1;
	Uart::writeBlocking((uint8_t*) "Card initialized\n\r", 18);

	bool success = 1;

	for (int i = 0; i < 1000 && success; i++)
	{
		sdio.writeBlocks(&writeBuffer[i], 1, i);
		sdio.readBlocks(testBuffer, 1, i);

		for (int j = 0; j < 512; j++)
		{
			if(testBuffer[j] != writeBuffer[i+j])
			{
				success = 0;
				break;
			}
		}
	}

	if(success)
	{
		Uart::writeBlocking((uint8_t*) "1000 consecutive single block blocking writes: PASSED\n\r", 55);
	} else
	{
		Uart::writeBlocking((uint8_t*) "1000 consecutive single block blocking writes: ERROR\n\r", 54);
	}

	success = 1;
	for (int i = 0; i < 1000 && success; i++)
	{
		sdio.writeBlocks(&writeBuffer[i], 15, i*15+1789);
		sdio.readBlocks(testBuffer, 15, i*15+1789);

		for (int j = 0; j < 512*15; j++)
		{
			if(testBuffer[j] != writeBuffer[i+j])
			{
				success = 0;
				break;
			}
		}
	}

	if(success)
	{
		Uart::writeBlocking((uint8_t*) "1000 consecutive 15 multi block blocking writes: PASSED\n\r", 57);
	} else
	{
		Uart::writeBlocking((uint8_t*) "1000 consecutive 15 multi block blocking writes: ERROR\n\r", 56);
	}

	success = 1;
	for (int i = 0; i < 1000 && success; i++)
	{
		sdio.writeBlocksDMA(&writeBuffer[i*4 % (512*5)], 15, i*15+3117);
		while(!sdio.isTransferComplete());
		sdio.readBlocksDMA(testBuffer, 15, i*15+3117);
		while(!sdio.isTransferComplete());

		for (int j = 0; j < 512*15; j++)
		{
			if(testBuffer[j] != writeBuffer[i*4 % (512*5)+j])
			{
				success = 0;
				break;
			}
		}
	}

	if(success)
	{
		Uart::writeBlocking((uint8_t*) "1000 consecutive 15 multi block writes: PASSED\n\r", 48);
	} else
	{
		Uart::writeBlocking((uint8_t*) "1000 consecutive 15 multi block writes: ERROR\n\r", 47);
	}

	//PHASE 2 TESTING:---------------------------------------
	//-------------------------------------------------------

	Uart::writeBlocking((uint8_t*) "Start Phase2 testing\n\r", 22);

	FATFS fat;
	FIL fil;            /* File object */
	FRESULT res;        /* API result code */
	UINT bw;
	res = f_mount(&fat, "", 1);

	res = f_mkfs("", FM_FAT32, 0, (void*) writeBuffer, 512);
	if(res)
	{
		Uart::writeBlocking((uint8_t*) "Failed Making Filesystem. Stopped\n\r", 35);
		while(1);
	}
	Uart::writeBlocking((uint8_t*) "Created Filesystem: PASSED\n\r", 28);

	for(int i = 0; i < 10240; i++)
	{
		writeBuffer[i] = (xorshift32() % 89) + 33;
	}

	success = 1;
	for (uint8_t i = 0; i < 100 && success; i++)
	{
		char nameBuffer[32] = {0};
		uint8_t num = UIntToString(i, nameBuffer);
		nameBuffer[num]='.';
		nameBuffer[num+1]='t';
		nameBuffer[num+2]='x';
		nameBuffer[num+3]='t';
		res = f_open(&fil, nameBuffer, FA_CREATE_NEW | FA_WRITE);
		if (res)
		{
			success = 0;
			break;
		}

		res = f_write(&fil, writeBuffer, 10240, &bw);
		if (res || bw != 10240)
		{
			success = 0;
			break;
		}

		res = f_close(&fil);
		if(res)
		{
			success = 0;
			break;
		}
	}

	if(success)
	{
		Uart::writeBlocking((uint8_t*) "Create Files: PASSED\n\r", 23);
	} else
	{
		Uart::writeBlocking((uint8_t*) "Failed to create File\n\r", 24);
	}

	success = 1;
	for (uint8_t i = 0; i < 100 && success; i++)
	{
		char nameBuffer[32] = {0};
		uint8_t num = UIntToString(i, nameBuffer);
		nameBuffer[num]='.';
		nameBuffer[num+1]='t';
		nameBuffer[num+2]='x';
		nameBuffer[num+3]='t';
		res = f_open(&fil, nameBuffer, FA_READ);
		if (res)
		{
			success = 0;
			break;
		}

		res = f_read(&fil, testBuffer, 10240, &bw);
		if (res || bw != 10240)
		{
			success = 0;
			break;
		}

		for (int j = 0; j < 10240; j++)
		{
			if(testBuffer[j] != writeBuffer[j])
			{
				success = 0;
				break;
			}
		}

		res = f_close(&fil);
		if(res)
		{
			success = 0;
			break;
		}
	}

	if(success)
	{
		Uart::writeBlocking((uint8_t*) "Read Files: PASSED\n\r", 21);
	} else
	{
		Uart::writeBlocking((uint8_t*) "Failed to read File\n\r", 22);
	}



	while(1)
	{
		//Sck::toggle();
		//Data0::toggle();
		Board::LedGreen::toggle();
		xpcc::delayMilliseconds(100);
	}
}
