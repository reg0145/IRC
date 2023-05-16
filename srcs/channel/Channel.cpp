#include "Channel.hpp"

Channel::Channel(std::string channelName)
{
	_channelName = channelName;
}

Channel::~Channel()
{
}

void Channel::addClient(Client* client)
{
	_clients[client->getNickname()] = client;
}

void Channel::removeClient(Client* client)
{
	std::string nickname = client->getNickname();
	std::map<std::string, Client*>::iterator it = _clients.find(nickname);

	if (it != _clients.end())
	{
		_clients.erase(nickname);
	}
}

void Channel::removeClientByNickname(std::string nickname)
{
	std::map<std::string, Client*>::iterator it = _clients.find(nickname);

	if (it != _clients.end())
	{
		_clients.erase(nickname);
	}
}

void Channel::addOperator(std::string nickname)
{
	_operator.push_back(nickname);
}

void Channel::removeOperator(std::string nickname)
{
	_operator.remove(nickname);
}

bool Channel::isOperator(std::string nickname)
{
	std::list<std::string>::iterator it;
	for (it = _operator.begin(); it != _operator.end(); it++)
	{
		if (*it == nickname)
		{
			return true;
		}
	}
	return false;
}

bool Channel::isClientInChannel(std::string nickname)
{
	std::map<std::string, Client*>::iterator it = _clients.find(nickname);

	if (it != _clients.end())
	{
		return true;
	}
	return false;
}

std::list<std::string> Channel::getClientsName()
{
	std::list<std::string> clientsName;

	std::map<std::string, Client*>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		clientsName.push_back(it->first);
	}
	return clientsName;
}

std::string &Channel::getChannelName()
{
	return _channelName;
}

std::string Channel::getTopic()
{
	return _topic;
}

int Channel::getClientCount()
{
	return _clients.size();
}

std::map<std::string, Client*> &Channel::getClients()
{
	return _clients;
}

void Channel::setTopic(std::string topic)
{
	_topic = topic;
}
