#ifndef PACKET_HPP
#define PACKET_HPP

#include "../message/IRCMessage.hpp"

struct RecvPacketInfo
{
	int sessionIndex;
	IRCMessage &message;
};

#endif
