// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_TCS3414_HPP
#	error	"Don't include this file directly, use 'tcs3414.hpp' instead!"
#endif

template<typename I2cMaster>
typename xpcc::Tcs3414<I2cMaster>::Data xpcc::Tcs3414<I2cMaster>::data;

template<typename I2cMaster>
typename xpcc::tcs3414::Rgb xpcc::Tcs3414<I2cMaster>::color;

template < typename I2cMaster >
xpcc::Tcs3414<I2cMaster>::Tcs3414(uint8_t address)
: I2cDevice<I2cMaster,2>(address),
  commandBuffer{0,0,0,0}
{
}

// ----------------------------------------------------------------------------
template<typename I2cMaster>
xpcc::ResumableResult<bool>
xpcc::Tcs3414<I2cMaster>::configure(
		const Gain gain,
		const Prescaler prescaler,
		const IntegrationMode mode,
		const uint8_t time)
{
	RF_BEGIN();

	if ( RF_CALL(setGain(gain, prescaler)) )
	{
		if ( RF_CALL(setIntegrationTime(mode, time)) )
		{
			RF_RETURN(true);
		}
	}

	RF_END_RETURN(false);
}

// ----------------------------------------------------------------------------
// MARK: - Tasks
template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Tcs3414<I2cMaster>::refreshAllColors()
{
	RF_BEGIN();

	if ( RF_CALL(readRegisters(
			RegisterAddress::DATA1LOW,
			data.dataBytes,
			sizeof(data.dataBytes)
	) ) )
	{
		// adapt the values to the overall light intensity
		// so that R + G + B = C
		color.red	= data.red.get();
		color.green	= data.green.get();
		color.blue	= data.blue.get();

		{
			// START --> This part is not really necessary
			// Rationale:
			// Imagine a low band light. For example a green laser. In case the filters
			// of this sensors do not transfer this wavelength well, it might
			// result in all colors being very low. The clear value will not
			// filter colors and thus it will see a bright light (intensity).
			// In order to still have some signal the very low green value can be
			// amplified with the clear value.
			const float c =	static_cast<float>(color.red) +
					static_cast<float>(color.green) +
					static_cast<float>(color.blue);
			const float f = data.clear.get() / c;
			color.red	*= f;
			color.green	*= f;
			color.blue	*= f;
		}

		// <-- END
		RF_RETURN(true);
	}

	RF_END_RETURN(false);
}

// ----------------------------------------------------------------------------
template<typename I2cMaster>
xpcc::ResumableResult<bool>
xpcc::Tcs3414<I2cMaster>::writeRegister(
		const RegisterAddress address,
		const uint8_t value)
{
	RF_BEGIN();

	commandBuffer[0] =
				0x80							// write command
			|	0x40							// with SMB read/write protocol
			|	static_cast<uint8_t>(address);	// at this address
	commandBuffer[2] = value;

	this->transaction.configureWrite(commandBuffer, 3);

	RF_END_RETURN_CALL( this->runTransaction() );
}

template<typename I2cMaster>
xpcc::ResumableResult<bool>
xpcc::Tcs3414<I2cMaster>::readRegisters(
		const RegisterAddress address,
		uint8_t* const values,
		const uint8_t count)
{
	RF_BEGIN();

	commandBuffer[0] =
			static_cast<uint8_t>(0x80)		// write command
			| static_cast<uint8_t>(0x40)		// with SMB read/write protocol
			| static_cast<uint8_t>(address);	// at this address

	this->transaction.configureWriteRead(commandBuffer, 1, values, count);

	RF_END_RETURN_CALL( this->runTransaction() );
}
