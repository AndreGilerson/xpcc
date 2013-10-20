// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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
 */
// ----------------------------------------------------------------------------
#ifndef XPCC_TCPIP__SERVER_HPP
#define XPCC_TCPIP__SERVER_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <list>

#include "tcpip_connection.hpp"

namespace xpcc
{
	namespace tcpip
	{

	    /**
	     *  This class handles the communication between all components via tcp /ip
	     *  All clients register to on the server. The server keeps track of all alive
	     *  client processes and distributes all messages according to the xpcc message
	     *  header.
	     *
	     *  \author Thorsten Lajewski
	     */
		class Server
		{
		public:
			Server();

			//~Server();

			void spawnReceiveConnection();

			void  distribute();

			void update();



		private:

			void accept_handler(boost::shared_ptr<xpcc::tcpip::Connection> receive,
					const boost::system::error_code& error);

			boost::shared_ptr<boost::asio::io_service> ioService;
			boost::asio::ip::tcp::endpoint endpoint;
			boost::asio::ip::tcp::acceptor acceptor;

			std::list<boost::shared_ptr<xpcc::tcpip::Connection> > receiveConnections;

		};
	}
}
#endif // XPCC_TCPIP__SERVER_HPP
