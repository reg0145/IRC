#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include <vector>
#include <string>
#include "../client/Client.hpp"

class Channel
{
	public:
		Channel(std::string channelName);
		~Channel();

		void addClient(Client *client);
		void removeClient(Client *client);
		std::vector<std::string> getClientsName();
		std::string getOperatorName();
		std::string getChannelName();
		int getClientCount();

	private:
		std::string _channelName;
		std::string _operator;
		std::map<std::string, Client*> _clients;
};

#endif
