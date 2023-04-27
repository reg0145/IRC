#include "ChannelManager.hpp"

ChannelManager::ChannelManager()
{
}

ChannelManager::~ChannelManager()
{
}

void ChannelManager::checkChannel(std::string channelName)
{
	Channel *channel = getChannel(channelName);

	if (channel)
	{
		/* todo: join */
	}
	else
	{
		addChannel(channelName);
	}
}

void ChannelManager::addChannel(std::string channelName)
{
	_channels[channelName] = new Channel(channelName);
}

void ChannelManager::removeChannel(std::string channelName)
{
	Channel *channel = getChannel(channelName);

	if (channel)
	{
		_channels.erase(channelName);
		delete channel;
	}
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
