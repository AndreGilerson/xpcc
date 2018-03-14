#include <xpcc/architecture/platform.hpp>
#include <xpcc/architecture/interface/gpio.hpp>

#include <stm32l4xx.h>
#include "sdio1.h"

#include <fatfs/diskio.h>
#include <fatfs/ff.h>

#include <xpcc/architecture/platform.hpp>
#include <xpcc/processing/timer.hpp>
#include <xpcc/driver/temperature/ds18b20.hpp>
#include <xpcc/driver/display/ssd1306.hpp>
#include <xpcc/architecture/interface/uart.hpp>
#include <xpcc/architecture/platform/driver/one_wire/uart/one_wire_master.hpp>

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

using namespace xpcc::stm32;
using OneWirePin = Board::A0;

using Sda = Board::D14;
using Scl = Board::D15;
using MyI2cMaster = xpcc::SoftwareI2cMaster<Board::D15, Board::D14>;
xpcc::Ssd1306<MyI2cMaster> display;

class Environment{
public:
	Environment() : dirty(false)
	{}

public:
	int16_t tempA;
	int16_t tempB;
	int16_t tempC;
	int16_t tempD;

	bool dirty;
};

Environment environment;

class ThreadDisplay : public xpcc::pt::Protothread
{
public:
	ThreadDisplay()
	{
	}

	bool
	update()
	{
		PT_BEGIN();

		while(true)
		{
				// we wait until the task started
				if (PT_CALL(display.ping())) {
						break;
				}
				// otherwise, try again in 100ms
				this->timeout.restart(100);
				PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		XPCC_LOG_INFO << "[disp] Display responded" << xpcc::endl;

		display.initializeBlocking();
		display.setFont(xpcc::font::Assertion);
		display << "Hello Steffen!";
		display.update();

		while(true)
		{
			display.clear();
//			display << "Hello Steffen!!" << counter++ << xpcc::endl;
			display.printf("Temp A = %3d.%02d\n", environment.tempA / 100, environment.tempA % 100);
			display.printf("Temp B = %3d.%02d\n", environment.tempB / 100, environment.tempB % 100);
			display.printf("Temp C = %3d.%02d\n", environment.tempC / 100, environment.tempC % 100);
			display.printf("Temp D = %3d.%02d\n", environment.tempD / 100, environment.tempD % 100);

			display.update();

			PT_YIELD();
		}

		PT_END();
	}

protected:
	xpcc::ShortTimeout timeout;
	xpcc::Ssd1306<MyI2cMaster, /* Height */ 64> display;

	uint8_t counter;
};

class ThreadThermometer : public xpcc::pt::Protothread
{
public:
	ThreadThermometer(): ds18b20A(romA), ds18b20B(romB), ds18b20C(romC), ds18b20D(romD)
	{
	}

	//--------------------------------------------------------------------------
	bool
	update()
	{
		PT_BEGIN();


		while(true)
		{
			XPCC_LOG_INFO << "[1w] Init:" << xpcc::endl;

			ow.initialize();

			if (ow.touchReset()) {
				XPCC_LOG_INFO << "[1w] devices found." << xpcc::endl;
				break;
			}

			XPCC_LOG_INFO << "[1w] No devices found!" << xpcc::endl;

			// otherwise, try again in 100ms
			this->timeout.restart(100);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		// search for connected DS18B20 devices
		ow.resetSearch(0x28);

		if (ow.searchNext(romA)) {
			XPCC_LOG_INFO << "[1w] found1: " << xpcc::hex;
			for (uint8_t ii = 0; ii < 8; ++ii) {
				XPCC_LOG_INFO << romA[ii];
			}
			XPCC_LOG_INFO << xpcc::ascii << xpcc::endl;
			xpcc::delayMilliseconds(50);
		}
		if (ow.searchNext(romB)) {
			XPCC_LOG_INFO << "[1w] found2: " << xpcc::hex;
			for (uint8_t ii = 0; ii < 8; ++ii) {
				XPCC_LOG_INFO << romB[ii];
			}
			XPCC_LOG_INFO << xpcc::ascii << xpcc::endl;
			xpcc::delayMilliseconds(50);
		}
		if(ow.searchNext(romC)) {
			XPCC_LOG_INFO << "[1w] found3: " << xpcc::hex;
			for (uint8_t ii = 0; ii < 8; ++ii) {
				XPCC_LOG_INFO << romC[ii];
			}
			XPCC_LOG_INFO << xpcc::ascii << xpcc::endl;
			xpcc::delayMilliseconds(50);
		}
		if(ow.searchNext(romD)) {
			XPCC_LOG_INFO << "[1w] found4: " << xpcc::hex;
			for (uint8_t ii = 0; ii < 8; ++ii) {
				XPCC_LOG_INFO << romD[ii];
			}
			XPCC_LOG_INFO << xpcc::ascii << xpcc::endl;
			xpcc::delayMilliseconds(50);
		}

		XPCC_LOG_INFO << "[1w] Search finished!" << xpcc::endl;

		ds18b20A.setIdentifier(romA);
		ds18b20B.setIdentifier(romB);
		ds18b20C.setIdentifier(romC);
		ds18b20D.setIdentifier(romD);

		//depending on configuration, conversion check should be modified
		XPCC_LOG_INFO << "configuration" << xpcc::endl;
		ds18b20A.configure(0x00, 0x00, 12);
		xpcc::delayMilliseconds(50);
		ds18b20B.configure(0x00, 0x00, 12);
		xpcc::delayMilliseconds(50);
		ds18b20C.configure(0x00, 0x00, 12);
		xpcc::delayMilliseconds(50);
		ds18b20D.configure(0x00, 0x00, 12);
		xpcc::delayMilliseconds(50);

		XPCC_LOG_INFO << "configuration done!" << xpcc::endl;

		while (true)
		{
			{
				// read the temperature from a connected DS18B20
				PT_CALL(ds18b20A.startConversion()); // call ow.writebyte

				XPCC_LOG_DEBUG << "startconversion A" << xpcc::endl;

				this->timeout.restart(750);
				PT_WAIT_UNTIL(this->timeout.isExpired());

				if (ds18b20A.isConversionDone())
				{
					{
						XPCC_LOG_DEBUG << "before readtemp A" << xpcc::endl;
						temperatureA = 0;
						tempA = &temperatureA;
						readTempCountA = 5;

						do {
							if(PT_CALL(ds18b20A.readTemperature(tempA)) == true){
								XPCC_LOG_INFO << "TemperatureA: " << temperatureA << xpcc::endl;
								environment.tempA = temperatureA;
								break;
							}
							else{
								XPCC_LOG_WARNING << "failed to read A, readTempCount is " << readTempCountA << xpcc::endl;
								readTempCountA--;
							}
						}while ( !(readTempCountA == 0) );
					}
						xpcc::delayMilliseconds(100);
				}
			}
			{
				// read the temperature from a connected DS18B20
				PT_CALL(ds18b20B.startConversion()); // call ow.writebyte

				XPCC_LOG_DEBUG << "startconversion B" << xpcc::endl;

				this->timeout.restart(750);
				PT_WAIT_UNTIL(this->timeout.isExpired());

				if (ds18b20B.isConversionDone())
				{
					{
						XPCC_LOG_DEBUG << "before readtemp B" << xpcc::endl;
						temperatureB = 0;
						tempB = &temperatureB;
						readTempCountB = 5;

						do {
							if(PT_CALL(ds18b20B.readTemperature(tempB)) == true){
								XPCC_LOG_INFO << "TemperatureB: " << temperatureB << xpcc::endl;
								environment.tempB = temperatureB;
								break;
							}
							else{
								XPCC_LOG_WARNING << "failed to read B, readTempCount is " << readTempCountB << xpcc::endl;
								readTempCountB--;
							}
						}while ( !(readTempCountB == 0) );
					}
					xpcc::delayMilliseconds(100);
				}
			}
			{
				// read the temperature from a connected DS18B20
				PT_CALL(ds18b20C.startConversion()); // call ow.writebyte

				XPCC_LOG_DEBUG << "startconversion C" << xpcc::endl;

				this->timeout.restart(750);
				PT_WAIT_UNTIL(this->timeout.isExpired());

				if (ds18b20C.isConversionDone())
				{
					{
						XPCC_LOG_DEBUG << "before readtemp C" << xpcc::endl;
						temperatureC = 0;
						tempC = &temperatureC;
						readTempCountC = 5;

						do {
							if(PT_CALL(ds18b20C.readTemperature(tempC)) == true){
								XPCC_LOG_INFO << "TemperatureC: " << temperatureC << xpcc::endl;
								environment.tempC = temperatureC;
								break;
							}
							else{
								XPCC_LOG_WARNING << "failed to read C, readTempCount is " << readTempCountC << xpcc::endl;
								readTempCountC--;
							}
						}while ( !(readTempCountC == 0) );
					}
						xpcc::delayMilliseconds(100);
				}
			}
			{
				// read the temperature from a connected DS18B20
				PT_CALL(ds18b20D.startConversion()); // call ow.writebyte

				XPCC_LOG_DEBUG << "startconversion D" << xpcc::endl;

				this->timeout.restart(750);
				PT_WAIT_UNTIL(this->timeout.isExpired());

				if (ds18b20D.isConversionDone())
				{
					{
						XPCC_LOG_DEBUG << "before readtemp D" << xpcc::endl;
						temperatureD = 0;
						tempD = &temperatureD;
						readTempCountD = 5;

						do {
							if(PT_CALL(ds18b20D.readTemperature(tempD)) == true){
								XPCC_LOG_INFO << "TemperatureD: " << temperatureD << xpcc::endl;
								environment.tempD = temperatureD;
								break;
							}
							else{
								XPCC_LOG_WARNING << "failed to read D, readTempCount is " << readTempCountD << xpcc::endl;
								readTempCountD--;
							}
						}while ( !(readTempCountD == 0) );
					}
						xpcc::delayMilliseconds(100);
				}
			}
			PT_YIELD();
		}
		PT_END();
	}

protected:
	xpcc::ShortTimeout timeout;
	xpcc::UartOneWireMaster<Usart3, Board::systemClock> ow;

	uint8_t romA[8];
	uint8_t romB[8];
	uint8_t romC[8];
	uint8_t romD[8];

	xpcc::Ds18b20< xpcc::UartOneWireMaster<Usart3, Board::systemClock> > ds18b20A;
	xpcc::Ds18b20< xpcc::UartOneWireMaster<Usart3, Board::systemClock> > ds18b20B;
	xpcc::Ds18b20< xpcc::UartOneWireMaster<Usart3, Board::systemClock> > ds18b20C;
	xpcc::Ds18b20< xpcc::UartOneWireMaster<Usart3, Board::systemClock> > ds18b20D;

	int16_t temperatureA;
	int16_t *tempA;
	int8_t readTempCountA;

	int16_t temperatureB;
	int16_t *tempB;
	int8_t readTempCountB;

	int16_t temperatureC;
	int16_t *tempC;
	int8_t readTempCountC;

	int16_t temperatureD;
	int16_t *tempD;
	int8_t readTempCountD;

	int8_t conversionCheckA;
	int8_t conversionCheckB;
	int8_t conversionCheckC;
	int8_t conversionCheckD;

};

ThreadDisplay threadDisplay;
ThreadThermometer ThreadThermometer;

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

	threadDisplay.update();
	ThreadThermometer.update();
	xpcc::delayMicroseconds(1000);

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
		//Uart::writeBlocking((uint8_t*) "1000 consecutive single block blocking writes: PASSED\n\r", 55);
		XPCC_LOG_DEBUG << "1000 consecutive single block blocking writes: PASSED\n\r" << xpcc::endl;
	} else
	{
		//Uart::writeBlocking((uint8_t*) "1000 consecutive single block blocking writes: ERROR\n\r", 54);
		XPCC_LOG_DEBUG << "1000 consecutive single block blocking writes: ERROR\n\r" << xpcc::endl;
	}

	success = 1;
	for (int i = 0; i < 1000 && success; i++)
	{
		sdio.writeBlocks(&writeBuffer[i], 15, i*15+1789);
		sdio.readBlocks(testBuffer, 15, i*15+1789);

		for (int j = 0; j < 512*15; j++)
		{
			//XPCC_LOG_DEBUG << testBuffer[j] << writeBuffer[i+j] << xpcc::endl;
			if(testBuffer[j] != writeBuffer[i+j])
			{
				success = 0;
				break;
			}
		}
	}

	if(success)
	{
		//Uart::writeBlocking((uint8_t*) "1000 consecutive 15 multi block blocking writes: PASSED\n\r", 57);
		XPCC_LOG_DEBUG << "1000 consecutive 15 multi block blocking writes: PASSED\n\r" << xpcc::endl;
	} else
	{
		//Uart::writeBlocking((uint8_t*) "1000 consecutive 15 multi block blocking writes: ERROR\n\r", 56);
		XPCC_LOG_DEBUG << "1000 consecutive 15 multi block blocking writes: ERROR\n\r" << xpcc::endl;
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
		//Uart::writeBlocking((uint8_t*) "1000 consecutive 15 multi block writes: PASSED\n\r", 48);
		XPCC_LOG_DEBUG << "1000 consecutive 15 multi block blocking writes: PASSED\n\r" << xpcc::endl;
	} else
	{
		//Uart::writeBlocking((uint8_t*) "1000 consecutive 15 multi block writes: ERROR\n\r", 47);
		XPCC_LOG_DEBUG << "1000 consecutive 15 multi block blocking writes: ERROR\n\r" << xpcc::endl;
	}

	//PHASE 2 TESTING:---------------------------------------
	//-------------------------------------------------------

//	Uart::writeBlocking((uint8_t*) "Start Phase2 testing\n\r", 22);
	XPCC_LOG_DEBUG << "Start Phase2 testing\n\r" << xpcc::endl;

	FATFS fat;
	FIL fil;            // File object
	FRESULT res;        // API result code
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
