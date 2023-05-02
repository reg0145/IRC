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

	if (!channel)
	{
		if (addChannel(channelName) == FAIL)
		{
			return FAIL;
		}
		channel = getChannel(channelName);
		channel->addOperator(client->getNickname());
	}

	channel->addClient(client);
	client->addChannel(channelName);

	return SUCCESS;
}

int ChannelManager::leaveClient(std::string channelName, Client *client)
{
	Channel *channel = getChannel(client->getChannel(channelName));

	if (!channel)
	{
		return FAIL;
	}

	std::string nickname = client->getNickname();
	if (channel->isOperator(nickname))
	{
		channel->removeOperator(nickname);
	}

	channel->removeClient(client);
	client->removeChannel(channelName);

	if (channel->getClientCount() == 0)
	{
		removeChannel(channel);
	}
	return SUCCESS;
}

bool ChannelManager::isValidChannelName(std::string channelName)
{
	if (channelName[0] == '#' || channelName[0] == '&')
	{
		return true;
	}
	return false;
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

std::string ChannelManager::getChannelInfo(std::string channelName)
{
	std::string channelInfo;
	Channel *channel = getChannel(channelName);

	std::list<std::string> clientsName = channel->getClientsName();
	std::list<std::string>::iterator it;
	for (it = clientsName.begin(); it != clientsName.end(); it++)
	{
		if (channel->isOperator(*it))
		{
			channelInfo += "@" + *it + " ";
		}
		else
		{
			channelInfo += *it + " ";
		}
	}

	if (channelInfo.size() > 0)
	{
		channelInfo.pop_back();
	}
	
	return channelInfo;
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
