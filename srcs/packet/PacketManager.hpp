#ifndef PACKETMANAGER_HPP
#define PACKETMANAGER_HPP

#include <map>
#include "../packet/Packet.hpp"
#include "../message/IRCMessage.hpp"
#include "../channel/ChannelManager.hpp"
#include "../client/ClientManager.hpp"

class PacketManager
{
	public:
		void init(char* _password);
		void process(int sessionIndex, IRCMessage &req);

		void broadcastChannel(const std::string &channelName, std::string &res);
		void broadcastChannels(const std::set<std::string> &channelNames, std::string &res);
		void broadcastChannelWithoutMe(int sessionIndex, Channel *channel, std::string &res);
		void broadcastChannelsWithoutMe(int sessionIndex, const std::set<std::string> &channelNames, std::string &res);
		void processNick(int sessionIndex, IRCMessage &req);
		void processPass(int sessionIndex, IRCMessage &req);
		void processUser(int sessionIndex, IRCMessage &req);
		void processPing(int sessionIndex, IRCMessage &req);
		void processJoin(int sessionIndex, IRCMessage &req);
		void processPart(int sessionIndex, IRCMessage &req);
		void processTopic(int sessionIndex, IRCMessage &req);
		void processKick(int sessionIndex, IRCMessage &req);
		void processPrivmsg(int sessionIndex, IRCMessage &req);
		void processNotice(int sessionIndex, IRCMessage &req);
		void processQuit(int sessionIndex, IRCMessage &req);
		void processMode(int sessionIndex, IRCMessage &req);

		static void(*_sendPacketFunc)(int sessionIndex, std::string &res);

	private:
		char* _password;
		ClientManager _clientManager;
		ChannelManager _channelManager;
		typedef void(PacketManager::*PROCESS_RECV_PACKET_FUNCTION)(int, IRCMessage&);

		std::map<std::string, PROCESS_RECV_PACKET_FUNCTION> _recvFuntionDictionary;
};

#endif
