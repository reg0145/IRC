#ifndef PACKETMANAGER_HPP
#define PACKETMANAGER_HPP

#include <map>
#include <sys/socket.h>
#include "../packet/Packet.hpp"
#include "../message/IRCMessage.hpp"
#include "../channel/ChannelManager.hpp"
#include "../client/ClientManager.hpp"

class PacketManager
{
	public:
		void init();
		void process(int clientSocket, int sessionIndex, IRCMessage &message);
		
		void processDisconnect(int clientSocket, int sessionIndex, IRCMessage &message);
		void processNick(int clientSocket, int sessionIndex, IRCMessage &message);
		void processPass(int clientSocket, int sessionIndex, IRCMessage &message);
		void processUser(int clientSocket, int sessionIndex, IRCMessage &message);

	private:
		ClientManager _clientManager;
		ChannelManager _channelManager;
		typedef void(PacketManager::* PROCESS_RECV_PACKET_FUNCTION)(int, int, IRCMessage&);

		std::map<std::string, PROCESS_RECV_PACKET_FUNCTION> _recvFuntionDictionary;
};

#endif
