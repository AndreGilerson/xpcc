// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SOFTWARE_SIMPLE_SPI_HPP
#	error	"Don't include this file directly, use 'simple_spi.hpp' instead!"
#endif

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
uint8_t xpcc::SoftwareSpiSimpleMaster<SCK, MOSI, MISO, Baudrate>::operationMode(0);


// ----------------------------------------------------------------------------
template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
template< class clockSource, uint32_t baudrate, uint16_t tolerance >
void ALWAYS_INLINE
xpcc::SoftwareSpiSimpleMaster<SCK, MOSI, MISO, Baudrate>::initialize()
{
	SCK::reset();
	MOSI::reset();
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
void ALWAYS_INLINE
xpcc::SoftwareSpiSimpleMaster<SCK, MOSI, MISO, Baudrate>::setDataMode(DataMode mode)
{
	operationMode = (operationMode & ~0b11) | static_cast<uint8_t>(mode);
	SCK::set(operationMode & 0b10);
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
void ALWAYS_INLINE
xpcc::SoftwareSpiSimpleMaster<SCK, MOSI, MISO, Baudrate>::setDataOrder(DataOrder order)
{
	if (order == DataOrder::LsbFirst)
		operationMode |= 0b100;
	else
		operationMode &= ~0b100;
}
// ----------------------------------------------------------------------------

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
uint8_t
xpcc::SoftwareSpiSimpleMaster<SCK, MOSI, MISO, Baudrate>::writeReadBlocking(uint8_t data)
{
	uint8_t input = 0;

	for (uint_fast8_t ii = 0; ii < 8; ++ii)
	{
		// CPHA=1, sample on falling edge
		if (operationMode & 0b01)
			delay();

		// if LSB first
		if (operationMode & 0b100) {
			input >>= 1;
			MOSI::set(data & 0x01);
		}
		else {
			input <<= 1;
			MOSI::set(data & 0x80);
		}

		// CPHA=0, sample on rising edge
		if (!(operationMode & 0b01))
			delay();

		// CPOL=0 -> High, CPOL=1 -> Low
		SCK::set(!(operationMode & 0b10));

		// CPHA=1, sample on falling edge
		if (operationMode & 0b01)
			delay();

		// if LSB first
		if (operationMode & 0b100) {
			if (MISO::read()) input |= 0x80;
			data >>= 1;
		}
		else {
			if (MISO::read()) input |= 0x01;
			data <<= 1;
		}

		// CPHA=0, sample on rising edge
		if (!(operationMode & 0b01))
			delay();

		// CPOL=0 -> Low, CPOL=1 -> High
		SCK::set(operationMode & 0b10);
	}

	return input;
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
void ALWAYS_INLINE
xpcc::SoftwareSpiSimpleMaster<SCK, MOSI, MISO, Baudrate>::delay()
{
	xpcc::delayMicroseconds(delayTime);
}

template <typename SCK, typename MOSI, typename MISO, uint32_t Baudrate>
void
xpcc::SoftwareSpiSimpleMaster<SCK, MOSI, MISO, Baudrate>::transferBlocking(uint8_t *tx, uint8_t *rx, std::size_t length)
{
	uint8_t tx_byte = 0xff;
	uint8_t rx_byte;

	for (std::size_t i = 0; i < length; i++)
	{
		if (tx) tx_byte = tx[i];

		rx_byte = writeReadBlocking(tx_byte);

		if (rx) rx[i] = rx_byte;
	}
}
