// coding: utf-8
/* Copyright (c) 2016, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

//
// NUCLEO-L476RG
// Nucleo kit for STM32L476RG
// www.st.com/web/catalog/tools/FM146/LN1847/PF261636
//

#ifndef XPCC_STM32_NUCLEO_L476RG_HPP
#define XPCC_STM32_NUCLEO_L476RG_HPP

#include <xpcc/architecture/platform.hpp>
#include <xpcc/debug/logger.hpp>

using namespace xpcc::stm32;

namespace Board
{

/// STM32L4 running at 48MHz generated from the
/// internal Multispeed oscillator

// Dummy clock for devices
struct systemClock {
	static constexpr uint32_t Frequency = 48 * MHz1;
	static constexpr uint32_t Ahb  = Frequency;
	static constexpr uint32_t Apb1 = Frequency;
	static constexpr uint32_t Apb2 = Frequency;

	static constexpr uint32_t Spi1 = Apb2;

	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;

	static constexpr uint32_t Can1 = Apb1;

	static constexpr uint32_t Usart1 = Apb2;

	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Usart4 = Apb1;
	static constexpr uint32_t Usart5 = Apb1;

	static bool inline
	enable()
	{
		// set flash latency first because system already runs from MSI
		ClockControl::setFlashLatency(Frequency);

		ClockControl::enableMultiSpeedInternalClock(ClockControl::MsiFrequency::MHz48);

		xpcc::clock::fcpu     = Frequency;
		xpcc::clock::fcpu_kHz = Frequency / 1000;
		xpcc::clock::fcpu_MHz = Frequency / 1000000;
		xpcc::clock::ns_per_loop = 47; // 3000 / 64 = 31.125 = ~31ns per delay loop

		return true;
	}
};

// Button connects to GND
using Button = xpcc::GpioInverted<GpioInputC13>;

// User LD2
using LedGreen = GpioOutputA5;
using Leds = xpcc::SoftwareGpioPort< LedGreen >;

namespace stlink
{
using Tx = GpioOutputA2;
using Rx = GpioInputA3;
using Uart = Usart2;
}

inline void
initialize()
{
	systemClock::enable();
	xpcc::cortex::SysTickTimer::initialize<systemClock>();

	stlink::Tx::connect(stlink::Uart::Tx);
	stlink::Rx::connect(stlink::Uart::Rx, Gpio::InputType::PullUp);
	stlink::Uart::initialize<systemClock, 115200>(12);

	LedGreen::setOutput(xpcc::Gpio::Low);

	Button::setInput();
}

} // Board namespace

#endif	// XPCC_STM32_NUCLEO_L476RG_HPP
