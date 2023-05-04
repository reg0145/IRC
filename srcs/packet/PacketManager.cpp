#include "PacketManager.hpp"
#include <iostream>

void (*PacketManager::_sendPacketFunc)(int sessionIndex, std::string &res) = 0;

void PacketManager::init(char* password)
{
	_password = password;

	_recvFuntionDictionary["NICK"] = &PacketManager::processNick;
	_recvFuntionDictionary["PASS"] = &PacketManager::processPass;
	_recvFuntionDictionary["USER"] = &PacketManager::processUser;
	_recvFuntionDictionary["PING"] = &PacketManager::processPing;
	_recvFuntionDictionary["JOIN"] = &PacketManager::processJoin;
	_recvFuntionDictionary["PART"] = &PacketManager::processPart;
	_recvFuntionDictionary["PRIVMSG"] = &PacketManager::processPrivmsg;
	_recvFuntionDictionary["NOTICE"] = &PacketManager::processNotice;
	_recvFuntionDictionary["QUIT"] = &PacketManager::processQuit;
}

void PacketManager::process(int sessionIndex, IRCMessage &message)
{
	std::map<std::string, PROCESS_RECV_PACKET_FUNCTION>::iterator it;
	it = _recvFuntionDictionary.find(message._command);

	if (it == _recvFuntionDictionary.end())
	{
		return ;
	}

	(this->*(it->second))(sessionIndex, message);
}

/* 해당 채널에 브로드캐스트 */
void PacketManager::broadcastChannel(const std::string &channelName, std::string &res)
{
	Channel *channel = _channelManager.getChannel(channelName);

	if (!channel)
	{
		return ;
	}

	std::map<std::string, Client*>::iterator itClient;
	std::map<std::string, Client*> &clients = channel->getClients();

	for (itClient = clients.begin(); itClient != clients.end(); itClient++)
	{
		int sessionIndex = itClient->second->getSessionIndex();
		_sendPacketFunc(sessionIndex, res);
	}
}

/* 모든 채널에 브로드캐스트 */
void PacketManager::broadcastChannels(const std::set<std::string> &channelNames, std::string &res)
{
	std::set<std::string>::iterator itChannelName;

	for (itChannelName = channelNames.begin(); itChannelName != channelNames.end(); itChannelName++)
	{
		broadcastChannel(*itChannelName, res);
	}
}

void PacketManager::broadcastChannelWithoutMe(int sessionIndex, Channel *channel, std::string &res)
{
	std::map<std::string, Client*>::iterator itClient;
	std::map<std::string, Client*> &clients = channel->getClients();

	for (itClient = clients.begin(); itClient != clients.end(); itClient++)
	{
		int recieverIndex = itClient->second->getSessionIndex();
		if (sessionIndex != recieverIndex)
		{
			_sendPacketFunc(recieverIndex, res);
		}
	}
}

void PacketManager::broadcastChannelsWithoutMe(int sessionIndex, const std::set<std::string> &channelNames, std::string &res)
{
	std::set<std::string>::iterator itChannelName;

	for (itChannelName = channelNames.begin(); itChannelName != channelNames.end(); itChannelName++)
	{
		Channel *channel = _channelManager.getChannel(*itChannelName);

		broadcastChannelWithoutMe(sessionIndex, channel, res);
	}

}

void PacketManager::processPass(int sessionIndex, IRCMessage &req)
{

	IRCMessage message;
	Client* client = _clientManager.getClient(sessionIndex);

	if (client->getIsPass())
	{
		message._command = "462";
		message._trailing = "You may not reregister";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	if (req._parameters.size() != 1)
	{
		message._command = "461";
		message._trailing = "Not enough parameters";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	if (req._parameters[0] != _password)
	{
		message._command = "464";
		message._trailing = "Password incorrect!";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	client->setPassTrue();
}

void PacketManager::processNick(int sessionIndex, IRCMessage &req)
{
	IRCMessage message;

	if (_clientManager.isFailedPass(sessionIndex))
	{
		message._command = "ERROR";
		message._trailing = "Authentication failed";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	/* 파라미터 1개 이상 존재하는가?*/
	if (req._parameters.size() != 1)
	{
		message._command = "461";
		message._trailing = "Not enough parameters";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	/* 닉네임 유효성 검사 */
	std::string &newNickname = req._parameters[0];
	if (!_clientManager.isValidNickname(newNickname))
	{
		message._command = "432";
		message._parameters.push_back(newNickname);
		message._trailing = "Erroneous nickname";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	/* 닉네임 중복여부 확인 */
	if (_clientManager.isUsedNickname(newNickname))
	{
		message._command = "433";
		message._parameters.push_back(newNickname);
		message._trailing = newNickname;
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	/* 닉네임 처음 등록된 경우 */
	Client* client = _clientManager.getClient(sessionIndex);
	std::string oldNickname = client->getNickname();
	if (oldNickname == "")
	{
		_clientManager.changeNickname(sessionIndex, oldNickname, newNickname);
		message._command = "001";
		message._parameters.push_back(newNickname);
		message._trailing = "Welcome to the Internet Relay Network, " + newNickname;
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	/* 닉네임 이미 등록된 경우 */
	_clientManager.changeNickname(sessionIndex, oldNickname, newNickname);
	_channelManager.changeNickname(client, oldNickname, newNickname);

	/* - 가입된 채널의 유저들에게 NICK 변경 알림 */
	message._prefix = oldNickname + "!~b@" + client->getServername();
	message._command = "NICK";
	message._trailing = newNickname;
	std::string res = message.toString();

	std::set<std::string> joinedChannelNames = client->getChannels();
	if (joinedChannelNames.size() > 0)
	{
		broadcastChannelsWithoutMe(sessionIndex, joinedChannelNames, res);
	}

	_sendPacketFunc(sessionIndex, res);
}

void PacketManager::processUser(int sessionIndex, IRCMessage &req)
{
	IRCMessage message;

	if (_clientManager.isUnRegistedClient(sessionIndex))
	{
		//sendPacketFunc();
		return ;
	}

	if (req._parameters.size() != 3)
	{
		//sendPacketFunc();
		return ;
	}

	Client* client = _clientManager.getClient(sessionIndex);
	client->setUsername(req._parameters[0]);
	client->setHostname(req._parameters[1]);
	client->setServername(req._parameters[2]);
	client->setName(req._trailing);
}

void PacketManager::processPing(int sessionIndex, IRCMessage &req)
{
	if (req._parameters.size() != 1)
	{
		return ;
	}

	IRCMessage message;
	message._command = "PONG";
	message._parameters.push_back(req._parameters[0]);

	std::string res = message.toString();
	_sendPacketFunc(sessionIndex ,res);
}

void PacketManager::processJoin(int sessionIndex, IRCMessage &req)
{
	IRCMessage message;
	Client* client = _clientManager.getClient(sessionIndex);

	if (_clientManager.isUnRegistedClient(sessionIndex))
	{
		return ;
	}

	if (req._parameters.size() != 1)
	{
		message._command = "461";
		message._trailing = "Not enough parameters";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	std::list<std::string> channelNames = IRCMessage::split(req._parameters[0], ",");
	std::list<std::string>::iterator itChannelName;
	for (itChannelName = channelNames.begin(); itChannelName != channelNames.end(); itChannelName++)
	{
		if (_channelManager.isValidChannelName(*itChannelName) == false)
		{
			message._command = "403";
			message._parameters.push_back(*itChannelName);
			message._trailing = "No such channel";
			std::string res = message.toString();
			_sendPacketFunc(sessionIndex, res);
			return ;
		}
	}

	for (itChannelName = channelNames.begin(); itChannelName != channelNames.end(); itChannelName++)
	{
		message._parameters.clear();
		if (_clientManager.isJoinedChannel(sessionIndex, *itChannelName))
		{
			message._command = "443";
			message._parameters.push_back(client->getNickname());
			message._parameters.push_back(*itChannelName);
			message._trailing = "is already on channel";
			std::string res = message.toString();
			_sendPacketFunc(sessionIndex, res);
			continue ;
		}
		if (_channelManager.enterClient(*itChannelName, client) == FAIL)
		{
			/* new Channel 실패(malloc 실패) 코드 */
			return ;
		}
		message._command = "353";
		message._parameters.push_back(client->getNickname());
		message._parameters.push_back("=");
		message._parameters.push_back(*itChannelName);
		message._trailing = _channelManager.getChannelInfo(*itChannelName);
		std::string res = message.toString();
		broadcastChannel(*itChannelName, res);
	}
}

void PacketManager::processPart(int sessionIndex, IRCMessage &req)
{
	IRCMessage message;
	Client* client = _clientManager.getClient(sessionIndex);

	if (_clientManager.isUnRegistedClient(sessionIndex))
	{
		return ;
	}

	if (req._parameters.size() != 1)
	{
		message._command = "461";
		message._trailing = "Not enough parameters";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	std::list<std::string> channelNames = IRCMessage::split(req._parameters[0], ",");
	std::string nickname = client->getNickname();
	std::list<std::string>::iterator itChannelName;
	for (itChannelName = channelNames.begin(); itChannelName != channelNames.end(); itChannelName++)
	{
		memset(&message, 0, sizeof(IRCMessage));
		Channel* channel = _channelManager.getChannel(*itChannelName);
	
		if (!channel)
		{
			message._command = "403";
			message._parameters.push_back(nickname);
			message._parameters.push_back(*itChannelName);
			message._trailing = "No such channel";
			std::string res = message.toString();
			_sendPacketFunc(sessionIndex, res);
			continue ;
		}

		if (!_clientManager.isJoinedChannel(sessionIndex, *itChannelName))
		{
			message._command = "442";
			message._parameters.push_back(nickname);
			message._parameters.push_back(*itChannelName);
			message._trailing = "You're not on that channel";
			std::string res = message.toString();
			_sendPacketFunc(sessionIndex, res);
			continue ;
		}

		_channelManager.leaveClient(*channel, client);

		message._prefix = nickname + "!" + client->getUsername() + "@" + client->getServername();
		message._command = "PART";
		message._parameters.push_back(*itChannelName);
		message._trailing = req._trailing.size() ? req._trailing : "Leaving";
		std::string res = message.toString();

		_sendPacketFunc(sessionIndex, res);
		broadcastChannel(*itChannelName, res);
	}
}

void PacketManager::processPrivmsg(int sessionIndex, IRCMessage &req)
{
	IRCMessage message;
	std::string nickname = _clientManager.getClient(sessionIndex)->getNickname();

	if (_clientManager.isUnRegistedClient(sessionIndex))
	{
		return;
	}
	if (req._parameters.size() != 1)
	{
		message._command = "411";
		message._trailing = "No recipient given (PRIVMSG)";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return;
	}
	if (req._trailing == "")
	{
		message._command = "412";
		message._trailing = "No text to send";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return;
	}
	std::list<std::string> targets = IRCMessage::split(req._parameters[0], ",");
	for (std::list<std::string>::const_iterator it = targets.begin(); it != targets.end(); ++it)
	{
		memset(&message, 0, sizeof(IRCMessage));
		if (_channelManager.isValidChannelName(*it))
		{
			Channel* channel = _channelManager.getChannel(*it);
			if (!channel)
			{
				message._command = "401";
				message._parameters.push_back(*it);
				message._trailing = "No such nick/channel";
				std::string res = message.toString();
				_sendPacketFunc(sessionIndex, res);
				continue;
			}
			if (!channel->isClientInChannel(nickname))
			{
				message._command = "404";
				message._parameters.push_back(*it);
				message._trailing = "Cannot send to channel";
				std::string res = message.toString();
				_sendPacketFunc(sessionIndex, res);
				continue;
			}
			message._prefix = nickname;
			message._command = "PRIVMSG";
			message._parameters.push_back(*it);
			message._trailing = req._trailing;
			std::string res = message.toString();
			broadcastChannelWithoutMe(sessionIndex, channel, res);
		}
		else
		{
			Client* client = _clientManager.getClientByNickname(*it);
			if (!client)
			{
				message._command = "401";
				message._parameters.push_back(*it);
				message._trailing = "No such nick/channel";
				std::string res = message.toString();
				_sendPacketFunc(sessionIndex, res);
				continue;
			}
			message._prefix = nickname;
			message._command = "PRIVMSG";
			message._parameters.push_back(*it);
			message._trailing = req._trailing;
			std::string res = message.toString();
			_sendPacketFunc(client->getSessionIndex(), res);
		}
	}
}

void PacketManager::processNotice(int sessionIndex, IRCMessage &req)
{
	IRCMessage message;
	std::string nickname = _clientManager.getClient(sessionIndex)->getNickname();

	if (_clientManager.isUnRegistedClient(sessionIndex))
	{
		return;
	}
	if(req._parameters.size() != 1)
	{
		return;
	}
	if (req._trailing == "")
	{
		return;
	}
	std::list<std::string> targets = IRCMessage::split(req._parameters[0], ",");
	for (std::list<std::string>::const_iterator it = targets.begin(); it != targets.end(); ++it)
	{
		memset(&message, 0, sizeof(IRCMessage));
		if (_channelManager.isValidChannelName(*it))
		{
			Channel* channel = _channelManager.getChannel(*it);
			if (!channel)
			{
				continue;
			}
			if (!channel->isClientInChannel(nickname))
			{
				continue;
			}
			message._prefix = nickname;
			message._command = "NOTICE";
			message._parameters.push_back(*it);
			message._trailing = req._trailing;
			std::string res = message.toString();
			broadcastChannelWithoutMe(sessionIndex, channel, res);
		}
		else
		{
			Client* client = _clientManager.getClientByNickname(*it);
			if (!client)
			{
				continue;
			}
			message._prefix = nickname;
			message._command = "NOTICE";
			message._parameters.push_back(*it);
			message._trailing = req._trailing;
			std::string res = message.toString();
			_sendPacketFunc(client->getSessionIndex(), res);
		}
	}
}

void PacketManager::processQuit(int sessionIndex, IRCMessage &req)
{
	Client* client = _clientManager.getClient(sessionIndex);

	if (_clientManager.isUnRegistedClient(sessionIndex))
	{
		client->leaveClient();
		return ;
	}

	/* 가입된 채널의 유저들에게 QUIT 알림 */
	IRCMessage message;
	std::set<std::string>::iterator itChannelName;
	std::set<std::string> channelNames = client->getChannels();
	for (itChannelName = channelNames.begin(); itChannelName != channelNames.end(); itChannelName++)
	{
		Channel* channel = _channelManager.getChannel(*itChannelName);
		if (!channel)
		{
			continue;
		}
		_channelManager.leaveClient(*channel, client);
		message._prefix = client->getNickname() + "!" + client->getUsername() + "@" + client->getServername();
		message._command = "QUIT";
		message._trailing = req._trailing;
		std::string res = message.toString();
		broadcastChannel(*itChannelName, res);
	}

	_clientManager.removeClient(sessionIndex);

	std::cout << ">> client[" << sessionIndex << "] Disconnected <<" << std::endl;
}
