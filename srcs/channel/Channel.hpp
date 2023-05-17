#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include <list>
#include <string>
#include "ChannelMode.hpp"
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
		void addInvitedClient(int sessionIndex);
		void removeInvitedClient(int sessionIndex);

		bool isOperator(std::string nickname);
		bool isModeOn(int mode);
		bool isLimitOver();
		bool isPasswordTrue(std::string password);
		bool isClientInChannel(std::string nickname);
		bool isInvitedClient(int sessionIndex);

		void setPassword(std::string password);
		void setTopic(std::string topic);
		void setLimit(std::string limit);
		void setMode(int mode);
		void unSetMode(int mode);

		std::string getTopic();
		std::list<std::string> getClientsName();
		std::string &getChannelName();
		int getClientCount();
		std::map<std::string, Client*> &getClients();

	private:
		std::string _channelName;
		std::list<std::string> _operator;
		std::map<std::string, Client*> _clients;
		std::set<int> _invitedClients;
		std::string _topic;
		std::string _password;
		std::list<std::string> _operator;
		std::map<std::string, Client*> _clients;
		unsigned long _limit;
		int _mode;
};

#endif
