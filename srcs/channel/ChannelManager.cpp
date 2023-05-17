#include "ChannelManager.hpp"

ChannelManager::ChannelManager()
{
}

ChannelManager::~ChannelManager()
{
}

Channel* ChannelManager::createChannel(std::string channelName)
{
	if (addChannel(channelName) == FAIL)
	{
		return NULL;
	}
	return getChannel(channelName);
}

void ChannelManager::enterClient(Channel &channel, Client* client)
{
	channel.addClient(client);
	client->addChannel(channel.getChannelName());
}

void ChannelManager::leaveClient(Channel &channel, Client* client)
{
	std::string nickname = client->getNickname();

	if (channel.isOperator(nickname))
	{
		channel.removeOperator(nickname);
	}

	channel.removeClient(client);
	client->removeChannel(channel.getChannelName());

	if (channel.getClientCount() == 0)
	{
		removeChannel(&channel);
	}
}

bool ChannelManager::isValidChannelName(std::string channelName)
{
	if ((channelName[0] != '#' && channelName[0] != '&') || !(channelName.size() > 1 && channelName.size() < 20))
	{
		return false;
	}
	return true;
}

bool ChannelManager::isValidChannelLimit(std::string &channelLimit)
{
	char *end;
	long limit = strtol(channelLimit.c_str(), &end, 10);

	if (limit < 1 || limit > INT_MAX || *end != '\0')
	{
		return false;
	}

	return true;
}
bool ChannelManager::isValidChannelPassword(std::string &channelPassword)
{
	if (channelPassword.size() > 20)
	{
		return false;
	}
	return true;
}

void ChannelManager::changeNickname(Client* client, std::string oldNickname, std::string newNickname)
{
	std::set<std::string>::iterator itChannelName;
	std::set<std::string> channels = client->getChannels();

	for (itChannelName = channels.begin(); itChannelName != channels.end(); itChannelName++)
	{
		Channel* channel = getChannel(*itChannelName);
		if (channel)
		{
			channel->removeOperator(oldNickname);
			channel->addOperator(newNickname);
			channel->removeClientByNickname(oldNickname);
			channel->addClient(client);
		}
	}
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

void ChannelManager::removeChannel(Channel* channel)
{
	_channels.erase(channel->getChannelName());
	delete channel;
}

std::string ChannelManager::getChannelInfo(std::string channelName)
{
	std::string channelInfo;
	Channel* channel = getChannel(channelName);

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
