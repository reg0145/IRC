#include "Channel.hpp"

Channel::Channel(std::string channelName)
{
	_channelName = channelName;
}

Channel::~Channel()
{
}

void Channel::addClient(std::string nickname, Client *client)
{
	_clients[nickname] = client;
}

void Channel::removeClient(std::string nickname)
{
	std::map<std::string, Client*>::iterator it;
	it = _clients.find(nickname);

	if (it != _clients.end())
	{
		_clients.erase(nickname);
	}
}

std::string Channel::getChannelName()
{
	return _channelName;
}

