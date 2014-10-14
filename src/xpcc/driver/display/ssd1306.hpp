// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SSD1306_HPP
#define XPCC_SSD1306_HPP

#include <xpcc/ui/display/buffered_graphic_display.hpp>
#include <xpcc/processing/protothread.hpp>
#include <xpcc/processing/coroutine.hpp>
#include <xpcc/architecture/peripheral/i2c_device.hpp>
#include <xpcc/processing/periodic_timer.hpp>

namespace xpcc
{

struct ssd1306
{
	enum class
	Rotation : bool
	{
		Normal,
		UpsideDown
	};

	enum class
	ScrollStep : uint8_t
	{
		Frames2 = 0b111,
		Frames3 = 0b100,
		Frames4 = 0b101,
		Frames5 = 0b000,
		Frames25 = 0b110,
		Frames64 = 0b001,
		Frames128 = 0b010,
		Frames256 = 0b011
	};

public:
	enum class
	ScrollDirection : bool
	{
		Right,
		Left,
	};
};


/**
 * Driver for SSD1306 based OLED-displays using I2C.
 * This display is only rated to be driven with 400kHz, which limits
 * the frame rate to about 40Hz.
 *
 * @author	Niklas Hauser
 * @ingroup	lcd
 */
template < class I2cMaster >
class Ssd1306 : public ssd1306, public xpcc::I2cDevice<I2cMaster>,
				public xpcc::co::NestedCoroutine<1>, public BufferedGraphicDisplay<128, 64>
{
	enum Command : uint8_t
	{
		// fundamental commands
		SetContrastControl = 0x81,
		SetChargePump = 0x8D,
		SetEntireDisplayResumeToRam = 0xA4,
		SetEntireDisplayIgnoreRam = 0xA5,
		SetNormalDisplay = 0xA6,
		SetInvertedDisplay = 0xA7,
		SetDisplayOff = 0xAE,
		SetDisplayOn = 0xAF,

		// scrolling commands
		SetHorizontalScrollRight = 0x26,
		SetHorizontalScrollLeft = 0x27,
		SetVerticalAndHorizontalScrollRight = 0x29,
		SetVerticalAndHorizontalScrollLeft = 0x2A,
		SetDisableScroll = 0x2E,
		SetEnableScroll = 0x2F,
		SetVerticalScrollArea = 0xA3,

		// addressing commands
		SetLowerColumnStartAddress = 0x00,
		SetHigherColumnStartAddress = 0x10,
		SetMemoryMode = 0x20,
		SetColumnAddress = 0x21,
		SetPageAddress = 0x22,
		SetPageStartAddress = 0xB0,

		// Hardware configuration
		SetDisplayStartLine = 0x40,
		SetSegmentRemap0 = 0xA0,
		SetSegmentRemap127 = 0xA1,
		SetMultiplexRatio = 0xA8,
		SetComOutputScanDirectionIncrement = 0xC0,
		SetComOutputScanDirectionDecrement = 0xC8,
		SetDisplayOffset = 0xD3,
		SetComPins = 0xDA,

		// timing configuration
		SetDisplayClockDivideRatio = 0xD5,
		SetPreChargePeriod = 0xD9,
		SetV_DeselectLevel = 0xDB,
		Nop = 0xE3,
	};

	uint8_t
	i(ScrollDirection direction)
	{
		return static_cast<uint8_t>(direction);
	}

	uint8_t
	i(ScrollStep step)
	{
		return static_cast<uint8_t>(step);
	}

public:
	Ssd1306(uint8_t address = 0x3C);

	/// initializes for 3V3 with charge-pump
	bool inline
	initialize()
	{
		xpcc::co::Result<bool> result;
		while((result = initialize(this)).state > xpcc::co::NestingError)
			;
		return result.result;
	}

	/// Update the display with the content of the RAM buffer.
	virtual void
	update()
	{
		while(startWriteDisplay(this).state > xpcc::co::NestingError) ;
	}

	/// Use this method to synchronize writing to the displays buffer
	/// to avoid tearing.
	/// @return	`true` if the frame buffer is not being copied to the display
	bool ALWAYS_INLINE
	isWritable()
	{
		return (i2cTask != I2cTask::WriteDisplay);
	}

	// MARK: - TASKS
	/// pings the diplay
	xpcc::co::Result<bool>
	ping(void *ctx);

	/// initializes for 3V3 with charge-pump asynchronously
	xpcc::co::Result<bool>
	initialize(void *ctx);

	/// Starts a frame transfer to the display
	xpcc::co::Result<void>
	startWriteDisplay(void *ctx);

	// starts a frame transfer and waits for completion
	xpcc::co::Result<bool>
	writeDisplay(void *ctx);


	xpcc::co::Result<bool>
	invertDisplay(void *ctx, bool invert = true);

	xpcc::co::Result<bool>
	setContrast(void *ctx, uint8_t contrast = 0xCE);

	xpcc::co::Result<bool>
	setRotation(void *ctx, Rotation rotation=Rotation::Normal);


	xpcc::co::Result<bool>
	configureScroll(void *ctx, uint8_t origin, uint8_t size,
			ScrollDirection direction, ScrollStep steps);

	xpcc::co::Result<bool> ALWAYS_INLINE
	enableScroll(void *ctx)
	{ return writeCommand(ctx, Command::SetEnableScroll); }

	xpcc::co::Result<bool> ALWAYS_INLINE
	disableScroll(void *ctx)
	{ return writeCommand(ctx, Command::SetDisableScroll); }

protected:
	/// Write a command without data
	xpcc::co::Result<bool>
	writeCommand(void *ctx, uint8_t command);

	/// Write a command with one byte data
	xpcc::co::Result<bool>
	writeCommand(void *ctx, uint8_t command, uint8_t data);

	/// Write a command with two bytes data
	xpcc::co::Result<bool>
	writeCommand(void *ctx, uint8_t command, uint8_t data1, uint8_t data2);

private:
	bool
	startTransactionWithLength(uint8_t length);

	struct I2cTask
	{
		enum
		{
			Idle = 0,
			// insert all ssd1306::Command
			WriteDisplay = 0xFE,
			Ping = 0xFF
		};
	};

	class DataTransmissionAdapter : public xpcc::I2cWriteAdapter
	{
	public:
		DataTransmissionAdapter(uint8_t address)
		:	I2cWriteAdapter(address), control(0x40),
			buffer{0x80, 0, 0x80, 0, 0x80, 0}
		{
		}

		bool inline
		configureWrite(uint8_t (*buffer)[8], std::size_t size)
		{
			if (I2cWriteAdapter::configureWrite(&buffer[0][0], size))
			{
				control = 0xff;
				return true;
			}
			return false;
		}

	protected:
		virtual Writing
		writing()
		{
			// we first tell the display the column address again
			if (control == 0xff)
			{
				buffer[1] = Command::SetColumnAddress;
				buffer[5] = 127;
				control = 0xfe;
				return Writing(buffer, 6, OperationAfterWrite::Restart);
			}
			// then the page address. again.
			if (control == 0xfe)
			{
				buffer[1] = Command::SetPageAddress;
				buffer[5] = 7;
				control = 0xfd;
				return Writing(buffer, 6, OperationAfterWrite::Restart);
			}
			// then we set the D/C bit to tell it data is coming
			if (control == 0xfd)
			{
				control = 0x40;
				return Writing(&control, 1, OperationAfterWrite::Write);
			}

			// now we write the entire frame buffer into it.
			return Writing(I2cWriteAdapter::buffer, size, OperationAfterWrite::Stop);
		}

		uint8_t control;
		uint8_t buffer[6];
	};

private:
	volatile uint8_t i2cTask;
	volatile uint8_t i2cSuccess;
	uint8_t commandBuffer[14];
	bool initSuccessful;

	xpcc::I2cTagAdapter<xpcc::I2cWriteAdapter> adapter;
	xpcc::I2cTagAdapter<DataTransmissionAdapter> adapterData;
};

} // namespace xpcc

#include "ssd1306_impl.hpp"

#endif // XPCC_SSD1306_HPP
