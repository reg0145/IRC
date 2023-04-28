#include "Channel.hpp"

Channel::Channel(std::string channelName)
{
	_channelName = channelName;
}

Channel::~Channel()
{
}

void Channel::addClient(Client *client)
{
	_clients[client->getNickname()] = client;
}

void Channel::removeClient(Client *client)
{
	std::string nickname = client->getNickname();
	std::map<std::string, Client*>::iterator it = _clients.find(nickname);

	if (it != _clients.end())
	{
		it->second->leaveChannel(_channelName);
		_clients.erase(nickname);
	}
}

std::string Channel::getChannelName()
{
	return _channelName;
}

int Channel::getClientCount()
{
	return _clients.size();
}
