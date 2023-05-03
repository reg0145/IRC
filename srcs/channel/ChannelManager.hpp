#ifndef CHANNELMANAGER_HPP
#define CHANNELMANAGER_HPP

#include <map>
#include <string>
#include "Channel.hpp"
#include "../packet/PacketCode.hpp"

class ChannelManager
{
	public:
		ChannelManager();
		~ChannelManager();

		int enterClient(std::string channelName, Client* client);
		int leaveClient(std::string channelName, Client* client);
		bool isValidChannelName(std::string channelName);
		void changeNickname(Client* client, std::string oldNickname, std::string newNickname);
		std::string getChannelInfo(std::string channelName);

		Channel* getChannel(std::string channelName);

	private:
		std::map<std::string, Channel*> _channels;
		int addChannel(std::string channelName);
		void removeChannel(Channel* channel);
};

#endif
