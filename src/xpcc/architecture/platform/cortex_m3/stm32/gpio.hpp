// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_STM32__GPIO_HPP
#define XPCC_STM32__GPIO_HPP

#include <xpcc/architecture/driver/gpio.hpp>
#include "device.h"

namespace xpcc
{
	namespace stm32
	{
		/**
		 * \ingroup	stm32
		 * 
		 * Each port bit of the General Purpose IO (GPIO) Ports, can be
		 * individually configured by software in several modes:
		 * - Input floating
		 * - Input pull-up
		 * - Input-pull-down
		 * - Analog
		 * - Output open-drain
		 * - Output push-pull
		 * - Alternate function push-pull
		 * - Alternate function open-drain
		 * 
		 */
		enum InputMode
		{
			INPUT = 0x1,
			ANALOG = 0x0,
		};
		
		enum InputType
		{
			FLOATING = 0x4,
			PULLUP = 0x9,
			PULLDOWN = 0x8,
		};
		
		enum OutputMode
		{
			OUTPUT = 0x0,
			ALTERNATE = 0x8,
		};
		
		enum OutputType
		{
			PUSH_PULL = 0x0,
			OPEN_DRAIN = 0x4,
		};
		
		enum OutputSpeed
		{
			SPEED_2MHZ = 0x2,
			SPEED_10MHZ = 0x1,
			SPEED_50MHZ = 0x3,
		};
		
		template<unsigned int P, unsigned char N, bool = (N >= 8)>
		struct GpioMode {
			ALWAYS_INLINE static void setMode(uint32_t m) {
				reinterpret_cast<GPIO_TypeDef*> (P)->CRH &= ~(0xf << ((N - 8) * 4));
				reinterpret_cast<GPIO_TypeDef*> (P)->CRH |= m << ((N - 8) * 4);
			}
		};
		
		template<unsigned int P, unsigned char N>
		struct GpioMode<P, N, false> {
			ALWAYS_INLINE static void setMode(uint32_t m) {
				reinterpret_cast<GPIO_TypeDef*> (P)->CRL &= ~(0xf << (N * 4));
				reinterpret_cast<GPIO_TypeDef*> (P)->CRL |= m << (N * 4);
			}
		};
	}
}

/**
 * \ingroup	stm32
 * \brief	Create a input/output pin type
 * 
 * 
 */
#define	GPIO__IO(name, port, pin) \
	struct name { \
		ALWAYS_INLINE static void \
		configureOutput(::xpcc::stm32::OutputMode mode = ::xpcc::stm32::OUTPUT, \
				::xpcc::stm32::OutputType type = ::xpcc::stm32::PUSH_PULL, \
				::xpcc::stm32::OutputSpeed speed = ::xpcc::stm32::SPEED_50MHZ) { \
			uint32_t config = mode | type | speed; \
			::xpcc::stm32::GpioMode<GPIO ## port ## _BASE, pin>::setMode(config); \
		} \
		ALWAYS_INLINE static void setOutput() { configureOutput(); } \
		ALWAYS_INLINE static void setOutput(bool status) { \
			set(status); \
			setOutput(); } \
		ALWAYS_INLINE static void \
		configureInput(::xpcc::stm32::InputMode mode, \
				::xpcc::stm32::InputType type = ::xpcc::stm32::FLOATING) { \
			uint32_t config = 0; \
			if (mode != ::xpcc::stm32::ANALOG) { \
				config = (mode | type) & 0xc0; \
				if (type == ::xpcc::stm32::PULLUP) { \
					GPIO ## port->BSRR = (1 << pin); \
				} else { \
					GPIO ## port->BRR = (1 << pin); \
				} \
			} \
			::xpcc::stm32::GpioMode<GPIO ## port ## _BASE, pin>::setMode(config); \
		} \
		ALWAYS_INLINE static void setInput() { configureInput(::xpcc::stm32::INPUT); } \
		ALWAYS_INLINE static void set() { GPIO ## port->BSRR = (1 << pin); } \
		ALWAYS_INLINE static void reset() { GPIO ## port->BRR = (1 << pin); } \
		ALWAYS_INLINE static void toggle() { \
			if (GPIO ## port->IDR & (1 << pin)) { reset(); } else { set(); } } \
		ALWAYS_INLINE static bool read() { return (GPIO ## port->IDR & (1 << pin)); } \
		\
		ALWAYS_INLINE static void \
		set(bool status) { \
			if (status) { \
				set(); \
			} \
			else { \
				reset(); \
			} \
		} \
	}

/**
 * \brief	Create a output pin type
 * 
 * Examples:
 * \code
 * GPIO__OUTPUT(Led, C, 12);
 * 
 * Led::configure(xpcc::stm32::OUTPUT);
 * Led::configure(xpcc::stm32::OUTPUT, xpcc::stm32::PUSH_PULL);
 * Led::configure(xpcc::stm32::OUTPUT, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_10MHZ);
 * Led::configure(xpcc::stm32::OUTPUT, xpcc::stm32::OPEN_DRAIN);
 * 
 * Led::configure(xpcc::stm32::ALTERNATE, xpcc::stm32::PUSH_PULL); 
 * Led::configure(xpcc::stm32::ALTERNATE, xpcc::stm32::OPEN_DRAIN);
 * 
 * Led::set();
 * Led::reset();
 * 
 * Led::toggle();
 * \endcode
 * 
 * \ingroup	stm32
 */
#define	GPIO__OUTPUT(name, port, pin) \
	struct name { \
		ALWAYS_INLINE static void setOutput() { configure(::xpcc::stm32::OUTPUT); } \
		ALWAYS_INLINE static void setOutput(bool status) { \
			set(status); \
			setOutput(); } \
		ALWAYS_INLINE static void \
		configure(::xpcc::stm32::OutputMode mode = ::xpcc::stm32::OUTPUT, \
				::xpcc::stm32::OutputType type = ::xpcc::stm32::PUSH_PULL, \
				::xpcc::stm32::OutputSpeed speed = ::xpcc::stm32::SPEED_50MHZ) { \
			uint32_t config = mode | type | speed; \
			::xpcc::stm32::GpioMode<GPIO ## port ## _BASE, pin>::setMode(config); \
		} \
		ALWAYS_INLINE static void set() { GPIO ## port->BSRR = (1 << pin); } \
		ALWAYS_INLINE static void reset() { GPIO ## port->BRR = (1 << pin); } \
		ALWAYS_INLINE static void toggle() { \
			if (GPIO ## port->IDR & (1 << pin)) { reset(); } else { set(); } } \
		ALWAYS_INLINE static void \
		set(bool status) { \
			if (status) { \
				set(); \
			} \
			else { \
				reset(); \
			} \
		} \
	}

/**
 * \brief	Create a input type
 * 
 * Examples:
 * \code
 * GPIO__INPUT(Button, A, 0);
 * 
 * Button::configure(xpcc::stm32::INPUT);
 * Button::configure(xpcc::stm32::INPUT, xpcc::stm32::PULLUP);
 * Button::configure(xpcc::stm32::INPUT, xpcc::stm32::PULLDOWN);
 * Button::configure(xpcc::stm32::ANALOG);
 * 
 * if (Button::read()) {
 *     ...
 * }
 * \endcode
 * 
 * \ingroup	stm32
 */
#define GPIO__INPUT(name, port, pin) \
	struct name { \
		ALWAYS_INLINE static void \
		configure(::xpcc::stm32::InputMode mode, \
				::xpcc::stm32::InputType type = ::xpcc::stm32::FLOATING) { \
			uint32_t config = 0; \
			if (mode == ::xpcc::stm32::INPUT) { \
				config = (mode | type) & 0xc; \
				if (type == ::xpcc::stm32::PULLUP) { \
					GPIO ## port->BSRR = (1 << pin); \
				} else { \
					GPIO ## port->BRR = (1 << pin); \
				} \
			} \
			::xpcc::stm32::GpioMode<GPIO ## port ## _BASE, pin>::setMode(config); \
		} \
		ALWAYS_INLINE static void setInput() { configure(::xpcc::stm32::INPUT); } \
		ALWAYS_INLINE static bool read() { return (GPIO ## port->IDR & (1 << pin)); } \
	}

#endif // XPCC_STM32__GPIO_HPP
