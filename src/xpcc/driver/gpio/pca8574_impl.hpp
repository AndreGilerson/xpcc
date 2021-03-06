// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_PCA8574_HPP
#	error "Don't include this file directly, use 'pca8574.hpp' instead!"
#endif

template < class I2cMaster >
xpcc::Pca8574<I2cMaster>::Pca8574(uint8_t address):
	I2cDevice<I2cMaster, 2>(address),
	direction(Pins(0xff)), output(Pins(0xff)), input(Pins(0xff))
{
}

template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Pca8574<I2cMaster>::set(Pins pins)
{
	RF_BEGIN();

	// high is 1, low is 0
	// we can _always_ set the pins high
	output.set(pins);

	RF_END_RETURN_CALL( writePort(output.value) );
}

template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Pca8574<I2cMaster>::reset(Pins pins)
{
	RF_BEGIN();

	// high is 1, low is 0
	// only reset those that are actually configured as output
	output.reset(pins & direction);

	RF_END_RETURN_CALL( writePort(output.value) );
}

template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Pca8574<I2cMaster>::toggle(Pins pins)
{
	RF_BEGIN();

	// high is 1, low is 0
	// only toggle those that are actually configured as output
	output.toggle(pins & direction);

	RF_END_RETURN_CALL( writePort(output.value) );
}

template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Pca8574<I2cMaster>::set(Pins pins, bool value)
{
	RF_BEGIN();

	// high is 1, low is 0
	// only update those that are actually configured as output
	output.update(pins & direction, value);

	RF_END_RETURN_CALL( writePort(output.value) );
}

template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Pca8574<I2cMaster>::setInput(Pins pins)
{
	RF_BEGIN();

	// reset direction bits
	direction.reset(pins);
	// set pins high, which means input (open-drain, remember?)
	output.set(pins);

	RF_END_RETURN_CALL( writePort(output.value) );
}

template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Pca8574<I2cMaster>::writePort(uint8_t value)
{
	RF_BEGIN();

	output.value = value;
	this->transaction.configureWrite(&output.value, 1);

	RF_END_RETURN_CALL( this->runTransaction() );
};

template < class I2cMaster >
xpcc::ResumableResult<bool>
xpcc::Pca8574<I2cMaster>::readPort(uint8_t &value)
{
	RF_BEGIN();

	this->transaction.configureRead(&input.value, 1);
	if (RF_CALL(this->runTransaction()))
	{
		value = input.value;
		RF_RETURN( true );
	}

	RF_END_RETURN( false );
};
