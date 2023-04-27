#include "ClientManager.hpp"

ClientManager::ClientManager()
{
}

ClientManager::~ClientManager()
{
}

void ClientManager::checkClient(std::string nickname)
{
	Client *client = getClient(nickname);

	if (client)
	{
		/* 닉네임 중복 불가 error */
	}
	else
	{
		addClient(nickname);
	}
}

void ClientManager::addClient(std::string nickname)
{
	_clients[nickname] = new Client(nickname);
}

void ClientManager::removeClient(std::string nickname)
{
	Client *client = getClient(nickname);

	if (client)
	{
		_clients.erase(nickname);
		delete client;
	}
}

Client* ClientManager::getClient(std::string nickname)
{
	std::map<std::string, Client*>::iterator it;
	it = _clients.find(nickname);

	if (it == _clients.end())
	{
		return NULL;
	}

	return it->second;
}
