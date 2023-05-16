#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include <list>
#include <string>
#include "../client/Client.hpp"

class Channel
{
	public:
		Channel(std::string channelName);
		~Channel();

		void addClient(Client* client);
		void removeClient(Client* client);
		void removeClientByNickname(std::string nickname);
		void addOperator(std::string nickname);
		void removeOperator(std::string nickname);
		bool isOperator(std::string nickname);
		bool isClientInChannel(std::string nickname);

		std::list<std::string> getClientsName();
		std::string &getChannelName();
		std::string getTopic();
		int getClientCount();
		std::map<std::string, Client*> &getClients();
		void setTopic(std::string topic);

	private:
		std::string _channelName;
		std::string	_topic;
		std::list<std::string> _operator;
		std::map<std::string, Client*> _clients;
};

#endif
