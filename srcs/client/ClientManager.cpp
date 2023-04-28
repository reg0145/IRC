#include "ClientManager.hpp"

ClientManager::ClientManager()
{
	for (int i = 0; i < 1024; ++i)
	{
		Client* client = new(std::nothrow) Client(i);
		
		if (!client)
		{
			std::cerr << "ClientManager() : " << errno << std::endl;
			exit(EXIT_FAILURE);
		}
		
		_clientPool.push_back(client);
	}
}

ClientManager::~ClientManager()
{
}

bool ClientManager::checkNick(std::string nickname)
{
	std::map<std::string, Client*>::iterator it = _clients.find(nickname);
	
	if (it != _clients.end())
	{
		return FAIL;
	}
	return SUCCESS;
}

bool ClientManager::checkClient(int sessionIndex)
{	
	Client* client = getClient(sessionIndex);
	
	if (client->getNickname() == "" || !client->getIsPass())
	{
		return FAIL;
	}

	return SUCCESS;
}

void ClientManager::addClient(int sessionIndex, std::string nickname)
{
	_clients[nickname] = _clientPool[sessionIndex];
}

void ClientManager::removeClient(int sessionIndex)
{
	Client *client = getClient(sessionIndex);

	if (client)
	{
		_clients.erase(client->getNickname());
		client->leaveClient();
	}
}

Client* ClientManager::getClient(int sessionIndex)
{
	return _clientPool[sessionIndex];
}
