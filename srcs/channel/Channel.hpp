#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include <string>
#include "Client.hpp"

class Channel
{
	public:
		Channel(std::string channelName);
		~Channel();

		void addClient(std::string nickname, Client *client);
		void removeClient(std::string nickname);
		std::string getChannelName();

	private:
		std::string _channelName;
		std::string _operator;
		std::map<std::string, Client*> _clients;
};

#endif