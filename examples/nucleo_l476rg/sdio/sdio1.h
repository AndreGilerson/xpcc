#ifndef SDIO1_H
#define SDIO1_H

#include <xpcc/architecture/interface/spi_master.hpp>
#include "stm32l476xx.h"

class Sdio1
{
public:
	enum class
	BusWidth : uint32_t
	{
		BusWidth1B	= 0x00000000,
		BusWidth	= SDMMC_CLKCR_WIDBUS_1
	};

	enum class
	PowerSave : uint32_t
	{
		Enable	= 0x00000000,
		Disable	= SDMMC_CLKCR_PWRSAV
	};

	enum class
	ClockBypass : uint32_t
	{
		Disable	= 0x00000000,
		Enable	= SDMMC_CLKCR_BYPASS
	};

	static const uint32_t SD_CMD_CLEAR_MSK = SDMMC_CMD_CMDINDEX | SDMMC_CMD_WAITRESP |
										SDMMC_CMD_WAITINT | SDMMC_CMD_WAITPEND |
										SDMMC_CMD_CPSMEN | SDMMC_CMD_SDIOSUSPEND;

	enum class
	Command : uint32_t
	{
		SD_GO_IDLE_STATE		= 0,	//Resets SD card
		SD_GET_CID				= 2,
		SD_SEND_REL_ADD			= 3,
		SD_SELECT_CARD			= 7,
		SD_HS_SEND_EXT_CSD		= 8,
		SD_SEND_CSD				= 9,

		SD_STOP_TRANSMISSION	= 12,

		SD_SET_BLOCK_SIZE		= 16,
		SD_READ_SINGLE_BLOCK	= 17,
		SD_READ_MULTI_BLOCK		= 18,

		SD_WRITE_SINGLE_BLOCK	= 24,
		SD_WRITE_MULTI_BLOCK	= 25,

		SD_APP_CMD				= 55,
		SD_READ_OCR				= 58,

		SD_SD_APP_OP_COND		= 41,
		SD_SEND_STATUS			= 13,
		SD_SET_BUS_WIDTH		= 6,
	};

	static const uint32_t SD_CHECK_VOLTAGE	= 0x1AA;
	static const uint32_t SD_MAX_TRIAL		= 0xFFFF;
	static const uint32_t SD_RESP1_NO_ERROR	= 0xFDFFE008;
	static const uint32_t SD_VOLTAGE_WINDOW	= 0x80100000;
	static const uint32_t SD_HIGH_CAPACITY	= 0x40000000;

	enum class
	CommandResponse : uint32_t
	{
		None	= 0x00000000,
		Short	= SDMMC_CMD_WAITRESP_0,
		Long	= SDMMC_CMD_WAITRESP
	};

	enum class
	CommandError : uint8_t
	{
		Success	= 0,
		Timeout	= 1,
		CrcFail	= 2
	};

	enum class
	CardType : uint32_t
	{
		StdCapacityV1	= 0,
		HighCapacity,
		MMC
	};

protected:
	static CardType cardType;
	static uint32_t relativeAddress;
	static uint64_t deviceSize;
	static uint32_t sectorCount;
	static uint32_t maxTransferRate;

public:
	static bool transferComplete;
	static bool writeStarted;
	static bool readStarted;

public:
	Sdio1();

	static void powerOn();
	static void	powerOff();
	static uint8_t	initializeCard();

	static uint8_t readBlocks(const void* data, uint32_t numberOfBlocks, uint32_t blockAdress);
	static uint8_t readBlocksDMA(const void* data, uint32_t numberOfBlocks, uint32_t blockAdress);

	static uint8_t writeBlocks(const void* data, uint32_t numberOfBlocks, uint32_t blockAddress);
	static uint8_t writeBlocksDMA(const void* data, uint32_t numberOfBlocks, uint32_t blockAddress);

	static uint8_t getCardInfo();

	static bool	isTransferComplete();

	static CommandError	sendCommand(Command cmd, CommandResponse resp, uint32_t argument);
	static uint32_t	getCommandResp1();

	static uint32_t getSectorCount();
};

#endif
