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


class ClientManager {
	public:
		ClientManager();
		~ClientManager();

		bool checkNick(std::string nickname);
		bool checkClient(int sessionIndex);
		bool isJoinedChannel(int sessionIndex, std::string channelName);
		void addClient(int sessionIndex, std::string nickname);
		void removeClient(int sessionIndex);
		
		Client* getClient(int sessionIndex);

	private:
		std::vector<Client*> _clientPool;
		std::map<std::string, Client*> _clients;
};

#endif