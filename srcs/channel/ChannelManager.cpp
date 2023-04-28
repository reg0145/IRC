#include "ChannelManager.hpp"

ChannelManager::ChannelManager()
{
}

ChannelManager::~ChannelManager()
{
}

int ChannelManager::enterClient(std::string channelName, Client *client)
{
	Channel *channel = getChannel(channelName);

	if (channel)
	{
		channel->addClient(client);
	}
	else
	{
		if (addChannel(channelName) == FAIL)
		{
			return FAIL;
		}
		enterClient(channelName, client);
	}
	return SUCCESS;
}

int ChannelManager::leaveClient(std::string channelName, Client *client)
{
	Channel *channel = getChannel(client->getChannel(channelName));

	if (!channel)
	{
		return FAIL;
	}

	channel->removeClient(client);

	if (channel->getClientCount() == 0)
	{
		removeChannel(channel);
	}
	return SUCCESS;
}

int ChannelManager::addChannel(std::string channelName)
{
	Channel* channel = new(std::nothrow) Channel(channelName);

	if (!channel)
	{
		return FAIL;
	}

	_channels[channelName] = channel;
	return SUCCESS;
}

void ChannelManager::removeChannel(Channel *channel)
{
	_channels.erase(channel->getChannelName());
	delete channel;
}

Channel* ChannelManager::getChannel(std::string channelName)
{
	std::map<std::string, Channel*>::iterator it;
	it = _channels.find(channelName);

	if (it == _channels.end())
	{
		return NULL;
	}

	return it->second;
}
