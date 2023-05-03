#ifndef CLIENTMANAGER_HPP
#define CLIENTMANAGER_HPP

#include <map>
#include <vector>
#include <string>
#include <cerrno>
#include <iostream>
#include <cstdlib>
#include "../packet/PacketCode.hpp"
#include "Client.hpp"

class ClientManager
{
	public:
		ClientManager();
		~ClientManager();

		bool isUnRegistedClient(int sessionIndex);
		bool isFailedPass(int sessionIndex);
		bool isValidNickname(std::string nickname);
		bool isUsedNickname(std::string nickname);
		bool isJoinedChannel(int sessionIndex, std::string channelName);
		void changeNickname(int sessionIndex, std::string oldNickname, std::string newNickname);
		void addClient(int sessionIndex, std::string nickname);
		void removeClient(int sessionIndex);

		Client* getClient(int sessionIndex);
		Client* getClientByNickname(std::string nickname);

	private:
		std::vector<Client*> _clientPool;
		std::map<std::string, Client*> _clients;
};

#endif
