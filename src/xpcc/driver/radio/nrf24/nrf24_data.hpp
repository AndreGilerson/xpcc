// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__NRF24_DATA_HPP
#define XPCC__NRF24_DATA_HPP

#include <stdint.h>
#include <xpcc/debug/logger.hpp>
#include <xpcc/processing/timer.hpp>

#include "nrf24_phy.hpp"
#include "nrf24_config.hpp"
#include "nrf24_definitions.hpp"

#undef  XPCC_LOG_LEVEL
#define XPCC_LOG_LEVEL xpcc::log::DISABLED

namespace xpcc
{

/* Pipe layout:
 *
 * ===== 0 ===== : Used to receive ACKs when communicating directly with another node
 * ===== 1 ===== : Broadcast pipe, will determine upper 4 bytes of address of pipes 2 - 5
 * ===== 2 ===== : Own address
 * ===== 3 ===== : \
 * ===== 4 ===== :  | Separate connections to neighbouring nodes
 * ===== 5 ===== : /
 *
 */

/// @ingroup	nrf24
/// @author		Daniel Krebs
template<typename Nrf24Phy>
class Nrf24Data : xpcc::Nrf24Register
{
public:

	/// @{
	/// @ingroup	nrf24
	typedef uint64_t    BaseAddress;
	typedef uint8_t     Address;
	/// @}

	/// @ingroup	nrf24
	enum class SendingState
	{
		Busy,           ///< Waiting for ACK
		FinishedAck,    ///< Packet sent and ACK received
		FinishedNack,   ///< Packet sent but no ACK received in time
		DontKnow,       ///< When a packet was sent without requesting ACK
		Failed,         ///< Packet could not be sent
		Undefined       ///< Initial state before a packet has been handled
	};

	/// @{
	/// @ingroup	nrf24
	/// Data structure that user uses to pass data to the data layer
	struct ATTRIBUTE_PACKED Packet
	{
		Packet() :
			dest(0), src(0)
		{
			payload.length = Nrf24Phy::getMaxPayload();
		}

		struct ATTRIBUTE_PACKED Payload
		{
			uint8_t data[Nrf24Phy::getMaxPayload()];
			uint8_t length;      // max. 30!
		};

		Payload     payload;
		Address     dest;
		Address     src;         // will be ignored when sending
	};


	/// Header of Frame
	struct ATTRIBUTE_PACKED Header
	{
		uint8_t     src;
		uint8_t     dest;
	};

	/// Data that will be sent over the air
	struct ATTRIBUTE_PACKED Frame
	{
		Header      header;
		uint8_t     data[30];   // max. possible payload size (32 byte) - 2 byte (src + dest)
	};
	/// @}

public:

	/* typedef config and physical layer for simplicity */
	typedef xpcc::Nrf24Config<Nrf24Phy> Config;
	typedef Nrf24Phy Phy;


	static void
	initialize(BaseAddress base_address, Address own_address, Address broadcast_address = 0xFF);

public:

	/* general data layer interface */

	static bool
	sendPacket(Packet& packet);

	static bool
	getPacket(Packet& packet);

	static bool
	isReadyToSend();

	static bool
	isPacketAvailable();

	/*
	 * Returns true if the last packet has been fully processed, i.e. sending
	 * process is over (successful or not). Only return true one time per packet
	 */
	static bool
	isPacketProcessed()
	{
		if(packetProcessed)
		{
			packetProcessed = false;
			return true;
		}

		return false;
	}

	/*
	 * Call this function in your main loop
	 */
	static void
	update();

	/** Returns feedback for the last packet sent
	 *
	 */
	static SendingState
	getSendingFeedback()
	{ return state; }

	static Address
	getAddress()
	{ return ownAddress; }

	/** Set own address
	 *
	 */
	static void
	setAddress(Address address);

	static uint8_t
	getPayloadLength()
	{ return Phy::getPayloadLength() - sizeof(Header); }

	static Address
	getBroadcastAddress()
	{ return broadcastAddress; }

	/* nrf24 specific */

// not yet implemented
private:
	static bool
	establishConnection();

	static bool
	destroyConnection();


private:

	static inline uint64_t
	assembleAddress(Address address)
	{ return static_cast<uint64_t>((uint64_t)baseAddress | (uint64_t)address); }

	static bool
	updateSendingState();

private:

	/**  Base address of the network
	 *
	 *   The first 3 byte will be truncated, so the address is actually 5 bytes
	 *   long. The last byte will be used to address individual modules or
	 *   connections between them respectively. Use different base address for
	 *   separate networks, although it may impact performance seriously to run
	 *   overlapping networks.
	 *
	 *   Format:
	 *
	 *   | dont'care | base address | address |
	 *   | ---------------------------------- |
	 *   |   3 byte  |    4 byte    |  1 byte |
	 */
	static BaseAddress baseAddress;

	static Address broadcastAddress;

	static Address ownAddress;

	static Address connections[3];

	static Frame assemblyFrame;

	static SendingState state;

	static bool packetProcessed;

	/* This is a workaround because some times the radio module doesn't
	 * throw an interrupt after sending */
	static xpcc::Timeout sendingInterruptTimeout;
	static constexpr int interruptTimeoutAfterSending = 15; // in ms
};

}	// namespace xpcc

#include "nrf24_data_impl.hpp"

#endif /* XPCC__NRF24_DATA_HPP */
