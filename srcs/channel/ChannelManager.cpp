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
		/* client를 operator로 추가하는 코드 추가 */
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

bool ChannelManager::checkChannelName(std::string channelName)
{
	if (channelName[0] != '#' && channelName[0] != '&')
	{
		return FAIL;
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

std::string ChannelManager::getChannelInfo(std::string channelName)
{
	std::string channelInfo;
	Channel *channel = getChannel(channelName);

	std::string operatorName = channel->getOperatorName();
	if (operatorName != "")
	{
		channelInfo += "@" + operatorName + " ";
	}

	std::vector<std::string> clientsName = channel->getClientsName();
	std::vector<std::string>::iterator it;
	for (it = clientsName.begin(); it != clientsName.end(); it++)
	{
		channelInfo += *it + " ";
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
