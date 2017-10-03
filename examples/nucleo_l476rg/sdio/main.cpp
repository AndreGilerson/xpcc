#include <xpcc/architecture/platform.hpp>
#include <xpcc/architecture/interface/gpio.hpp>

#include <stm32l4xx.h>
#include "sdio1.h"

using Data0	= xpcc::stm32::GpioC8;
using Sck	= xpcc::stm32::GpioC12;
using Cmd	= xpcc::stm32::GpioD2;

using _Sdio = xpcc::stm32::SdioBase;

char testBuffer[1024];
char writeBuffer[1024];
int main()
{

	Board::initialize();

	Data0::connect(_Sdio::Data0);
	Sck::setOutput();
	Sck::connect(_Sdio::Sck, Gpio::OutputType::PushPull, Gpio::OutputSpeed::MHz50);
	Cmd::connect(_Sdio::Cmd);

	Sdio1 sdio;
	sdio.powerOn();

	if(sdio.initializeCard()) while(1);


	for(int i = 0; i < 1024; i++)
	{
		testBuffer[i] = 0;
	}

	for(int i = 0; i < 1024; i++)
	{
		writeBuffer[i] = i;
	}
	sdio.writeBlocks(writeBuffer, 2, 0);
	xpcc::delayMilliseconds(100);
	sdio.readBlocks(testBuffer, 2, 0);
	sdio.getCardInfo();
	while(1)
	{
		//Sck::toggle();
		//Data0::toggle();
		Board::LedGreen::toggle();
		xpcc::delayMilliseconds(100);
	}
}
