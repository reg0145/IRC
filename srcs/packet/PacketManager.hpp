#ifndef IRC_PACKETMANAGER_HPP
#define IRC_PACKETMANAGER_HPP

#include <map>
#include "../packet/Packet.hpp"
#include "../message/IRCMessage.hpp"


class PacketManager
{
	public:
		void init();
		//void process(RecvPacketInfo &packetInfo);
		void process(int sessionIndex, IRCMessage &message);
		void processNick(int sessionIndex, IRCMessage &message);

	private:
		typedef void(PacketManager::* PROCESS_RECV_PACKET_FUNCTION)(int, IRCMessage&);

		std::map<std::string, PROCESS_RECV_PACKET_FUNCTION> _recvFuntionDictionary;
};

#endif
