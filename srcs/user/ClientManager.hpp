#ifndef IRC_CLIENTMANAGER_HPP
#define IRC_CLIENTMANAGER_HPP

#include <map>
#include <string>
#include "Client.hpp"

/* session에 연결되면 유저 생성?? */
class ClientManager {
	public:
		ClientManager();
		~ClientManager();

		void checkClient(std::string nickname);
		void addClient(std::string nickname);
		void removeClient(std::string nickname);
		Client* getClient(std::string nickname);

	private:
		std::map<std::string, Client*> _clients;
};

#endif