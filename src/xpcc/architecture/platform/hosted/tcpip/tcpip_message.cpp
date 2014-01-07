#include "tcpip_message.hpp"

xpcc::tcpip::TCPHeader::TCPHeader(uint8_t sender):
		type(Type::REGISTER), header(), dataLength(0)
{
	this->header.source = sender;

}

xpcc::tcpip::TCPHeader::TCPHeader(xpcc::Header& header, int dataSize):
		type(Type::DATA), header(header), dataLength(dataSize)
{

}

xpcc::Header&
xpcc::tcpip::TCPHeader::getXpccHeader()
{
	return this->header;
}

xpcc::tcpip::TCPHeader::Type
xpcc::tcpip::TCPHeader::getType() const
{
	return this->type;
}

uint8_t
xpcc::tcpip::TCPHeader::getDataSize() const
{
	return this->dataLength;
}

bool
xpcc::tcpip::TCPHeader::isDataMessage() const
{
	return (this->getType() == TCPHeader::Type::DATA);
}

xpcc::tcpip::TCPHeader&
xpcc::tcpip::Message::getTCPHeader()
{
	return this->header;
}

xpcc::tcpip::Message::Message(xpcc::Header& header, SmartPointer payload):
		header(header, static_cast<int>(payload.getSize())), data(payload)
{

}


xpcc::tcpip::Message::Message(uint8_t identifier):
		header(identifier), data()
{
}


void
xpcc::tcpip::Message::encodeMessage()
{
	//deep copy message to char array
	char* header = reinterpret_cast<char*>(&this->getTCPHeader());

	for(int i = 0; i< this->getMessageLength(); ++i)
	{
		this->dataStorage[i] = 0;
	}


	for(int i = 0; i<xpcc::tcpip::TCPHeader::headerSize(); ++i)
	{
		this->dataStorage[i] = header[i];
	}

	if(this->getTCPHeader().getDataSize() > 0)
	{
		uint8_t* dataPtr = this->data.getPointer();

		for(int i = xpcc::tcpip::TCPHeader::headerSize(); i< this->getMessageLength(); ++i)
		{
			this->dataStorage[i] = static_cast<char>(dataPtr[i-xpcc::tcpip::TCPHeader::headerSize()]);
		}
	}
}

const char*
xpcc::tcpip::Message::getEncodedMessage() const
{
	return this->dataStorage;
}

xpcc::tcpip::Message::Message(const Message& msg):
 header(msg.header), data(msg.data)
{

}


int
xpcc::tcpip::Message::getMessageLength() const
{
	return xpcc::tcpip::TCPHeader::headerSize() + this-> header.getDataSize();
}

xpcc::Header&
xpcc::tcpip::Message::getXpccHeader()
{
	return this->header.getXpccHeader();
}
