#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include <list>
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
		void addOperator(std::string nickname);
		void removeOperator(std::string nickname);
		bool checkIsOperator(std::string nickname);
	
		std::vector<std::string> getClientsName();
		std::string &getChannelName();
		int getClientCount();

	private:
		std::string _channelName;
		std::list<std::string> _operator;
		std::map<std::string, Client*> _clients;
};

#endif
