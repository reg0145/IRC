#ifndef CHANNELMANAGER_HPP
#define CHANNELMANAGER_HPP

#include <map>
#include <new>
#include <string>
#include <cstdlib>
#include "Channel.hpp"
#include "../packet/PacketCode.hpp"

class ChannelManager
{
	public:
		ChannelManager();
		~ChannelManager();

		void enterClient(Channel &channel, Client* client);
		void leaveClient(Channel &channel, Client* client);
		bool isValidChannelName(std::string channelName);
		bool isValidChannelLimit(std::string &channelLimit);
		bool isValidChannelPassword(std::string &channelPassword);
		void changeNickname(Client* client, std::string oldNickname, std::string newNickname);
		std::string getChannelInfo(std::string channelName);

		Channel* createChannel(std::string channelName);
		Channel* getChannel(std::string channelName);

	private:
		std::map<std::string, Channel*> _channels;
		int addChannel(std::string channelName);
		void removeChannel(Channel* channel);
};

#endif
