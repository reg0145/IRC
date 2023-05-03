#include "ClientManager.hpp"

ClientManager::ClientManager()
{
	for (int i = 0; i < 1024; ++i)
	{
		Client* client = new (std::nothrow) Client(i);

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

bool ClientManager::isUnRegistedClient(int sessionIndex)
{
	Client* client = getClient(sessionIndex);

	if (client->getNickname() == "" || !client->getIsPass())
	{
		return true;
	}

	return false;
}

bool ClientManager::isFailedPass(int sessionIndex)
{
	Client* client = getClient(sessionIndex);

	if (client->getIsPass())
	{
		return false;
	}
	return true;
}

bool ClientManager::isValidNickname(std::string nickname)
{
	if (nickname.length() > 9)
	{
		return false;
	}

	for (std::string::size_type i = 0; i < nickname.length(); i++)
	{
		if (!std::isalnum(nickname[i]))
		{
			return false;
		}
	}

	return true;
}

bool ClientManager::isUsedNickname(std::string nickname)
{
	std::map<std::string, Client*>::iterator it = _clients.find(nickname);

	if (it == _clients.end())
	{
		return false;
	}
	return true;
}

bool ClientManager::isJoinedChannel(int sessionIndex, std::string channelName)
{
	Client* client = getClient(sessionIndex);

	if (client->getChannel(channelName) != "")
	{
		return true;
	}
	return false;
}

void ClientManager::changeNickname(int sessionIndex, std::string oldNickname, std::string newNickname)
{
	if (oldNickname != "")
	{
		_clients.erase(oldNickname);
	}

	addClient(sessionIndex, newNickname);
	_clients[newNickname]->setNickname(newNickname);
}

void ClientManager::addClient(int sessionIndex, std::string nickname)
{
	_clients[nickname] = _clientPool[sessionIndex];
}

void ClientManager::removeClient(int sessionIndex)
{
	Client* client = getClient(sessionIndex);

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

Client* ClientManager::getClientByNickname(std::string nickname)
{
	std::map<std::string, Client*>::iterator it;
	it = _clients.find(nickname);

	if (it == _clients.end())
	{
		return NULL;
	}

	return it->second;
}
