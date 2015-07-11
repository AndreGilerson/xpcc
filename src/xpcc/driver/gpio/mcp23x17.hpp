// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_MCP23X17_HPP
#define XPCC_MCP23X17_HPP

#include <xpcc/architecture/interface/gpio.hpp>
#include <xpcc/architecture/interface/register.hpp>
#include "mcp23_transport.hpp"

namespace xpcc
{

struct mcp23x17 : public Gpio
{
protected:
	/// @cond
	/// The addresses of the Configuration and Data Registers
	enum class
	Register : uint8_t
	{
		IODIR = 0x00,		///< Port direction (1=input, 0=output)
		IPOL = 0x02,		///< Invert polarity
		GPINTEN = 0x04,		///< Enable interrupt
		DEFVAL = 0x06,		///< Compare register for interrupt
		INTCON = 0x08,
		IOCON = 0x0A,		///< Configuration
		GPPU = 0x0C,		///< Enable pullups
		INTF = 0x0E,		///< Interrupt flag register
		INTCAP = 0x10,		///< Interrupt capture register
		GPIO = 0x12,		///< Port values
		OLAT = 0x14			///< Output latch register
	};

	enum class
	IoCon : uint8_t
	{
		Bank = Bit7,	///< Controls how the registers are addressed
		Mirror = Bit6,	///< INT Pins Mirror bit
		SeqOp = Bit5,	///< Sequential Operation mode bit
		DisSlw = Bit4,	///< Slew Rate control bit for SDA output
		HaEn = Bit3,	///< Hardware Address Enable bit
		Odr = Bit2,		///< This bit configures the INT pin as an open-drain output
		IntPol = Bit1	///< This bit sets the polarity of the INT output pin
	};
	XPCC_FLAGS8(IoCon);

	static constexpr uint8_t
	i(Register reg) { return uint8_t(reg); }
	/// @endcond

public:
	enum class
	Pin : uint16_t
	{
		A0 = (1 << 0),
		A1 = (1 << 1),
		A2 = (1 << 2),
		A3 = (1 << 3),
		A4 = (1 << 4),
		A5 = (1 << 5),
		A6 = (1 << 6),
		A7 = (1 << 7),

		B0 = (1 << 8),
		B1 = (1 << 9),
		B2 = (1 << 10),
		B3 = (1 << 11),
		B4 = (1 << 12),
		B5 = (1 << 13),
		B6 = (1 << 14),
		B7 = (1 << 15)
	};
	typedef xpcc::Flags16<Pin> Pins;
	XPCC_INT_TYPE_FLAGS(Pins);
}; // struct mcp23x17

/**
 * MCP23X15 16-Bit I/O Expander with Serial Interface
 *
 * GPB is the high byte, GPA the low byte.
 * The lower three address bits can be configured: 0100abc.
 *
 * Notice that you can specify multiple pins at the same time for functions
 * with argument type `Pins`, either by ORing the according pins, or
 * converting a 16bit value using the `Pins(uint16_t)` converting constructor.
 *
 * Other functions with argument type `Pin` can only take one pin.
 * If you want to operate on all 16bit, use the `get(Inputs|Outputs|Directions|Polarities)()`
 * getters.
 *
 * @tparam	Transport	Either the I2C or SPI Transport Layer.
 *
 * @see Mcp23TransportI2c
 * @see Mcp23TransportSpi
 *
 * @ingroup driver_gpio
 *
 * @author	Fabian Greif
 * @author	Niklas Hauser
 */
template <class Transport>
class Mcp23x17 : public mcp23x17, public Transport
{
public:
	/// Constructor, sets address to default of 0x20 (range 0x20 - 0x27)
	Mcp23x17(uint8_t address=0x20);

public:
	xpcc::ResumableResult<bool>
	setOutput(Pins pins);

	xpcc::ResumableResult<bool>
	set(Pins pins);

	xpcc::ResumableResult<bool>
	reset(Pins pins);

	xpcc::ResumableResult<bool>
	toggle(Pins pins);

	xpcc::ResumableResult<bool>
	update(Pins pins, bool value);

	bool
	isSet(Pin pin)
	{
		// high is 1, low is 0
		return memory.outputLatch.any(pin);
	}

	Direction
	getDirection(Pin pin)
	{
		// output is 0, input is 1
		return memory.direction.any(pin) ? Direction::In : Direction::Out;
	}

public:
	xpcc::ResumableResult<bool>
	setInput(Pins pins);

	xpcc::ResumableResult<bool>
	setPullUp(Pins pins);

	xpcc::ResumableResult<bool>
	resetPullUp(Pins pins);

	xpcc::ResumableResult<bool>
	setInvertInput(Pins pins);

	xpcc::ResumableResult<bool>
	resetInvertInput(Pins pins);

	bool
	read(Pin pin)
	{
		// high is 1, low is 0
		return memory.gpio.any(pin);
	}

public:
	xpcc::ResumableResult<bool> inline
	readInput()
	{ return Transport::read(i(Register::INTF), buffer + 14, 8); }

public:
	Pins inline
	getOutputs()
	{ return memory.outputLatch; }

	Pins inline
	getInputs()
	{ return memory.gpio; }

	/// 0 is input, 1 is output
	Pins inline
	getDirections()
	{ return ~memory.direction; }

	Pins inline
	getPolarities()
	{ return memory.polarity; }

private:
	/// @cond
	struct ATTRIBUTE_PACKED
	Memory
	{
		Memory() :
			direction(0xffff),
			polarity(0),
			interruptEnable(0),
			interruptDefault(0),
			interruptControl(0),
			controlA(IoCon(0)), controlB(IoCon(0)),
			pullup(0),
			interruptFlag(0),
			interruptCapture(0),
			gpio(0),
			outputLatch(0)
		{}

		Pins direction;			// IODIR
		Pins polarity;			// IPOL
		Pins interruptEnable;	// GPINTEN
		Pins interruptDefault;	// DEFVAL
		Pins interruptControl;	// INTCON
		IoCon controlA;			// IOCONA
		IoCon controlB;			// IOCONB
		Pins pullup;			// GPPU
		Pins interruptFlag;		// INTF
		Pins interruptCapture;	// INTCAP
		Pins gpio;				// GPIO
		Pins outputLatch;		// OLAT
	};

	union
	{
		Memory memory;
		uint8_t buffer[sizeof(Memory)];
	};
};

}	// namespace xpcc

#include "mcp23x17_impl.hpp"

#endif	// XPCC_MCP23X17_HPP
