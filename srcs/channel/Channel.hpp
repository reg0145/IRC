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
		bool isSetPassword();
		bool isSetTopic();
		bool isSetInvite();
		bool isSetLimit();
		bool isLimitOver();
		bool isPasswordTrue(std::string password);
		bool isClientInChannel(std::string nickname);

		void setIsInvite(bool isInvite);
		void setIsTopic(bool isTopic);
		void setPassword(std::string password);
		void setTopic(std::string topic);
		void setLimit(std::string limit);

		std::string getTopic();
		std::list<std::string> getClientsName();
		std::string &getChannelName();
		int getClientCount();
		std::map<std::string, Client*> &getClients();

	private:
		std::string _channelName;
		std::list<std::string> _operator;
		std::map<std::string, Client*> _clients;
		std::string _topic;
		std::string _password;
		unsigned long _limit;
		bool _isInvite;
		bool _isTopic;
};

#endif
