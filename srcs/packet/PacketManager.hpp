#ifndef PACKETMANAGER_HPP
#define PACKETMANAGER_HPP

#include <map>
#include <sys/socket.h>
#include "../packet/Packet.hpp"
#include "../message/IRCMessage.hpp"
#include "../channel/ChannelManager.hpp"
#include "../client/ClientManager.hpp"
#include "../session/SessionManager.hpp"

class PacketManager
{
	public:
		void init(char* _password);
		void process(int sessionIndex, IRCMessage &req);

		void broadcastChannel(const std::string &channelName, std::string &res);
		void broadcastChannels(const std::set<std::string> &channelNames, std::string &res);
		void broadcastChannelsWithoutMe(int sessionIndex, const std::set<std::string> &channelNames, std::string &res);
		void processDisconnect(int sessionIndex, IRCMessage &req);
		void processNick(int sessionIndex, IRCMessage &req);
		void processPass(int sessionIndex, IRCMessage &req);
		void processUser(int sessionIndex, IRCMessage &req);
		void processPing(int sessionIndex, IRCMessage &req);
		void processJoin(int sessionIndex, IRCMessage &req);
		void processPrivmsg(int sessionIndex, IRCMessage &req);

		static void(*_sendPacketFunc)(int sessionIndex, std::string &res);

	private:
		char* _password;
		ClientManager _clientManager;
		ChannelManager _channelManager;
		typedef void(PacketManager::*PROCESS_RECV_PACKET_FUNCTION)(int, IRCMessage&);

		std::map<std::string, PROCESS_RECV_PACKET_FUNCTION> _recvFuntionDictionary;
};

#endif
