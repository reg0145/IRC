#include "PacketManager.hpp"

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
	_recvFuntionDictionary["TOPIC"] = &PacketManager::processTopic;
	_recvFuntionDictionary["PRIVMSG"] = &PacketManager::processPrivmsg;
	_recvFuntionDictionary["NOTICE"] = &PacketManager::processNotice;
	_recvFuntionDictionary["QUIT"] = &PacketManager::processQuit;
	_recvFuntionDictionary["KICK"] = &PacketManager::processKick;
	_recvFuntionDictionary["MODE"] = &PacketManager::processMode;
	_recvFuntionDictionary["INVITE"] = &PacketManager::processInvite;
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

	/* 인증 확인 */
	if (_clientManager.isFailedPass(sessionIndex))
	{
		message._command = "ERROR";
		message._trailing = "Authentication failed";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	/* 파라미터 부족 확인 */
	if (req._parameters.size() != 3)
	{
		message._command = "461";
		message._trailing = "Not enough parameters";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	/* USER 재등록 확인 */
	Client* client = _clientManager.getClient(sessionIndex);
	if (client->getUsername() != "")
	{
		message._command = "462";
		message._trailing = "You may not reregister";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

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

	if (req._parameters.size() < 1 || req._parameters.size() > 2)
	{
		message._command = "461";
		message._trailing = "Not enough parameters";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	std::string nickname = client->getNickname();
	std::list<std::string> channelNames = IRCMessage::split(req._parameters[0], ",");
	std::list<std::string> passwords;

	if (req._parameters.size() == 2)
	{
		passwords = IRCMessage::split(req._parameters[1], ",");
	}

	std::list<std::string>::iterator itChannelName;
	std::list<std::string>::iterator itPassword = passwords.begin();
	for (itChannelName = channelNames.begin(); itChannelName != channelNames.end(); itChannelName++)
	{
		memset(&message, 0, sizeof(IRCMessage));
		if (!_channelManager.isValidChannelName(*itChannelName))
		{
			message._command = "403";
			message._parameters.push_back(nickname);
			message._parameters.push_back(*itChannelName);
			message._trailing = "No such channel";
			std::string res = message.toString();
			_sendPacketFunc(sessionIndex, res);
			return ;
		}

		if (_clientManager.isJoinedChannel(sessionIndex, *itChannelName))
		{
			continue ;
		}

		Channel *channel = _channelManager.getChannel(*itChannelName);
		std::string password = "";

		if (itPassword != passwords.end())
		{
			password = *itPassword;
			itPassword++;
		}
		if (!channel)
		{
			channel = _channelManager.createChannel(*itChannelName);
			if (!channel)
			{
				/* new Channel 실패(malloc 실패) 코드 */
				return ;
			}
			channel->addOperator(nickname);
		}
		else 
		{
			if (channel->isModeOn(MODE_PASSWORD) && !channel->isPasswordTrue(password))
			{
				message._command = "475";
				message._parameters.push_back(nickname);
				message._parameters.push_back(*itChannelName);
				message._trailing = "Cannot join channel (+k)";
				std::string res = message.toString();
				_sendPacketFunc(sessionIndex, res);
				continue ;
			}
			if (channel->isModeOn(MODE_INVITE_ONLY) && channel->isInvitedClient(sessionIndex))
			{
				message._command = "473";
				message._parameters.push_back(nickname);
				message._parameters.push_back(*itChannelName);
				message._trailing = "Cannot join channel (+i)";
				std::string res = message.toString();
				_sendPacketFunc(sessionIndex, res);
				continue ;
			}
			if (channel->isModeOn(MODE_LIMIT) && channel->isLimitOver())
			{
				message._command = "471";
				message._parameters.push_back(nickname);
				message._parameters.push_back(*itChannelName);
				message._trailing = "Cannot join channel (+l)";
				std::string res = message.toString();
				_sendPacketFunc(sessionIndex, res);
				continue ;
			}
		}

		_channelManager.enterClient(*itChannelName, client);
		
		message._prefix = nickname + "!" + client->getUsername() + "@" + client->getServername();
		message._command = "JOIN";
		message._parameters.push_back(*itChannelName);
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);

		std::string topic = channel->getTopic();
		if (topic != "")
		{
			memset(&message, 0, sizeof(IRCMessage));
			message._command = "332";
			message._parameters.push_back(nickname);
			message._parameters.push_back(*itChannelName);
			message._trailing = topic;
			res = message.toString();
			_sendPacketFunc(sessionIndex, res);
		}

		memset(&message, 0, sizeof(IRCMessage));
		message._command = "353";
		message._parameters.push_back(nickname);
		message._parameters.push_back("=");
		message._parameters.push_back(*itChannelName);
		message._trailing = _channelManager.getChannelInfo(*itChannelName);
		res = message.toString();
		broadcastChannel(*itChannelName, res);

		memset(&message, 0, sizeof(IRCMessage));
		message._command = "366";
		message._parameters.push_back(nickname);
		message._parameters.push_back(*itChannelName);
		message._trailing = "End of /NAMES list.";
		res = message.toString();
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
		if (!channel->isClientInChannel(nickname))
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

void PacketManager::processKick(int sessionIndex, IRCMessage &req)
{
	IRCMessage message;
	Client* client = _clientManager.getClient(sessionIndex);

	if (_clientManager.isUnRegistedClient(sessionIndex))
	{
		return ;
	}

	if (req._parameters.size() != 2)
	{
		message._command = "461";
		message._trailing = "Not enough parameters";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	std::string nickname = client->getNickname();
	std::string channelName = req._parameters[0];
	Channel* channel = _channelManager.getChannel(channelName);
	if (!channel)
	{
		message._command = "403";
		message._parameters.push_back(nickname);
		message._parameters.push_back(channelName);
		message._trailing = "No such channel";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}
	if (!channel->isClientInChannel(nickname))
	{
		message._command = "442";
		message._parameters.push_back(nickname);
		message._parameters.push_back(channelName);
		message._trailing = "You're not on that channel";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}
	if (!channel->isOperator(nickname))
	{
		message._command = "482";
		message._parameters.push_back(nickname);
		message._parameters.push_back(channelName);
		message._trailing = "You're not channel operator";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	std::list<std::string> targetNames = IRCMessage::split(req._parameters[1], ",");
	std::list<std::string>::iterator itTargetName;
	for (itTargetName = targetNames.begin(); itTargetName != targetNames.end(); itTargetName++)
	{
		memset(&message, 0, sizeof(IRCMessage));
		Client* target = _clientManager.getClientByNickname(*itTargetName);
		if (!channel->isClientInChannel(*itTargetName))
		{
			message._command = "441";
			message._parameters.push_back(channelName);
			message._parameters.push_back(*itTargetName);
			message._trailing = "They aren't on that channel";
			std::string res = message.toString();
			_sendPacketFunc(sessionIndex, res);
			continue ;
		}

		_channelManager.leaveClient(*channel, target);

		message._prefix = nickname + "!" + client->getUsername() + "@" + client->getServername();
		message._command = "KICK";
		message._parameters.push_back(channelName);
		message._parameters.push_back(*itTargetName);
		message._trailing = req._trailing.size() ? req._trailing : "Kicked";
		std::string res = message.toString();

		_sendPacketFunc(target->getSessionIndex(), res);
		broadcastChannel(channelName, res);
	}
}

void PacketManager::processTopic(int sessionIndex, IRCMessage &req)
{
	IRCMessage message;
	Client *client = _clientManager.getClient(sessionIndex);
	std::string nickname = client->getNickname();

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
	Channel *channel = _channelManager.getChannel(req._parameters[0]);
	std::string channelName = channel->getChannelName();
	if(!channel)
	{
		message._command = "403";
		message._parameters.push_back(nickname);
		message._parameters.push_back(channelName);
		message._trailing = "No such channel";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}
	if (!channel->isClientInChannel(nickname))
	{
		message._command = "442";
		message._parameters.push_back(nickname);
		message._parameters.push_back(channelName);
		message._trailing = "You're not on that channel";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}
	
	if (channel->isModeOn(MODE_TOPIC) && !channel->isOperator(nickname))
	{
		message._command = "482";
		message._parameters.push_back(channelName);
		message._trailing = "You're not channel operator";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}
	if (req._hasTrailing)
	{
		channel->setTopic(req._trailing);
		message._prefix = nickname + "!" + client->getUsername() + "@" + client->getServername();
		message._command = "332";
		message._parameters.push_back(nickname);
		message._parameters.push_back(channelName);
		message._trailing = req._trailing;
		message._hasTrailing = req._hasTrailing;
		std::string res = message.toString();
		broadcastChannelWithoutMe(sessionIndex, channel, res);
		
		memset(&message, 0, sizeof(IRCMessage));
		message._prefix = nickname + "!" + client->getUsername() + "@" + client->getServername();
		message._command = "333";
		message._parameters.push_back(nickname);
		message._parameters.push_back(channelName);
		message._parameters.push_back(nickname);
		std::ostringstream oss;
		oss << std::time(0);
		message._parameters.push_back(oss.str());
		res = message.toString();
		broadcastChannelWithoutMe(sessionIndex, channel, res);
	}
	else
	{
		std::string topic = channel->getTopic();
		if (topic == "")
		{
			message._command = "331";
			message._parameters.push_back(nickname);
			message._parameters.push_back(channelName);
			message._trailing = "No topic is set";
			std::string res = message.toString();
				_sendPacketFunc(sessionIndex, res);
			return;
		}
	}
	memset(&message, 0, sizeof(IRCMessage));

	message._prefix = nickname + "!" + client->getUsername() + "@" + client->getServername();
	message._command = "TOPIC";
	message._parameters.push_back(channelName);
	message._trailing = req._trailing;
	message._hasTrailing = req._hasTrailing;
	std::string res = message.toString();
	_sendPacketFunc(sessionIndex, res);
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

void PacketManager::processMode(int sessionIndex, IRCMessage &req)
{
	IRCMessage message;

	if (_clientManager.isUnRegistedClient(sessionIndex))
	{
		return;
	}

	/* 파라미터 개수 확인 */
	if (req._parameters.size() < 2)
	{
		message._command = "461";
		message._trailing = "Not enough parameters";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	std::string channelName = req._parameters[0];
	std::string channelMode = req._parameters[1];

	Client* client = _clientManager.getClient(sessionIndex);
	std::string nickname = client->getNickname();
	/* MODE <username> 명령어 처리하지 않음 */
	if (nickname == channelName)
	{
		return ;
	}

	/* 설정할 채널이 생성되었는지 확인 */
	Channel* channel = _channelManager.getChannel(channelName);
	if (!channel)
	{
		message._command = "403";
		message._parameters.push_back(channelName);
		message._trailing = "No such channel";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	/* 요청 클라이언트가 채널에 속해있는지 확인 */
	if (!channel->isClientInChannel(nickname))
	{
		message._command = "442";
		message._parameters.push_back(channelName);
		message._trailing = "You're not on that channel";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	/* 요청 클라이언트가 채널 운영자인지 확인 */
	if (!channel->isOperator(nickname))
	{
		message._command = "482";
		message._parameters.push_back(channelName);
		message._trailing = "You're not channel operator";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	/* 처리 가능한 Mode인지 확인*/
	if ((channelMode.length() != 2) ||
		!(channelMode[0] == '+' || channelMode[0] == '-'))
	{
		message._command = "472";
		message._parameters.push_back(channelMode);
		message._trailing = "is unknown mode char to me";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	char sign = channelMode[0];
	char mode = channelMode[1];

	switch (mode)
	{
		case 'i' :
			if (sign == '+')
			{
				channel->setMode(MODE_INVITE_ONLY);
			}
			else
			{
				channel->unSetMode(MODE_INVITE_ONLY);
			}
			break;
		case 't' :
			if (sign == '+')
			{
				channel->setMode(MODE_TOPIC);
			}
			else
			{
				channel->unSetMode(MODE_TOPIC);
			}
			break;
		case 'k' :
			if (sign == '+')
			{
				if (req._parameters.size() < 3)
				{
					message._command = "461";
					message._trailing = "Not enough parameters";
					std::string res = message.toString();
					_sendPacketFunc(sessionIndex, res);
					return ;
				}

				if (!_channelManager.isValidChannelPassword(req._parameters[2]))
				{
					message._command = "475";
					message._parameters.push_back(channelName);
					message._trailing = "Cannot join channel (+k)";
					std::string res = message.toString();
					_sendPacketFunc(sessionIndex, res);
					return ;
				}

				channel->setMode(MODE_PASSWORD);
				channel->setPassword(req._parameters[2]);

				/* 채널 사용자들에게는 KEY값 없이 알림 */
				message._prefix = nickname + "!" + client->getUsername() + "@" + client->getServername();
				message._command = req._command;
				message._parameters.push_back(req._parameters[1]);
				std::string res = message.toString();
				broadcastChannelWithoutMe(sessionIndex, channel, res);

				/* 채널 운영자에게는 KEY값 포함 알림 */
				message._parameters.push_back(req._parameters[2]);
				res = message.toString();
				_sendPacketFunc(sessionIndex, res);
				return ;
			}
			else
			{
				channel->unSetMode(MODE_PASSWORD);
				channel->setPassword("");
			}
			break;
		case 'o' :
			if (req._parameters.size() < 3)
			{
				message._command = "461";
				message._trailing = "Not enough parameters";
				std::string res = message.toString();
				_sendPacketFunc(sessionIndex, res);
				return ;
			}

			if (!channel->isClientInChannel(req._parameters[2]))
			{
				message._command = "442";
				message._parameters.push_back(channelName);
				message._trailing = "You're not on that channel";
				std::string res = message.toString();
				_sendPacketFunc(sessionIndex, res);
				return ;
			}

			if (sign == '+')
			{
				if (channel->isOperator(req._parameters[2]))
				{
					return ;
				}
				channel->addOperator(req._parameters[2]);
			}
			else
			{
				if (!channel->isOperator(req._parameters[2]))
				{
					return ;
				}
				channel->removeOperator(req._parameters[2]);
			}
			break;
		case 'l' :
			if (sign == '+')
			{
				if (req._parameters.size() < 3)
				{
					message._command = "461";
					message._trailing = "Not enough parameters";
					std::string res = message.toString();
					_sendPacketFunc(sessionIndex, res);
					return ;
				}

				if (!_channelManager.isValidChannelLimit(req._parameters[2]))
				{
					message._command = "472";
					message._parameters.push_back(channelMode);
					message._trailing = "is unknown mode char to me";
					std::string res = message.toString();
					_sendPacketFunc(sessionIndex, res);
					return ;
				}

				channel->setMode(MODE_LIMIT);
				channel->setLimit(req._parameters[2]);
			}
			else
			{
				channel->unSetMode(MODE_LIMIT);
				channel->setLimit("0");
			}
			break;
		default :
			message._command = "472";
			message._parameters.push_back(channelMode);
			message._trailing = "is unknown mode char to me";
			std::string res = message.toString();
			_sendPacketFunc(sessionIndex, res);
			return ;
	}

	message._prefix = nickname + "!" + client->getUsername() + "@" + client->getServername();
	message._command = req._command;
	message._parameters = req._parameters;
	std::string res = message.toString();
	broadcastChannel(channelName, res);
}

void PacketManager::processInvite(int sessionIndex, IRCMessage &req)
{
	IRCMessage message;

	/* 유저 등록 확인*/
	if (_clientManager.isUnRegistedClient(sessionIndex))
	{
		return ;
	}

	/* 파라미터 개수 확인 */
	if (req._parameters.size() != 2)
	{
		message._command = "461";
		message._trailing = "Not enough parameters";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	std::string inviteNickname = req._parameters[0];
	std::string inviteChannelName = req._parameters[1];

	/* 초대한 채널이 존재하는지 확인 */
	Channel* inviteChannel = _channelManager.getChannel(inviteChannelName);
	if (!inviteChannel)
	{
		message._command = "403";
		message._parameters.push_back(inviteChannelName);
		message._trailing = "No such channel";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	/* 초대자(inviter)가 채널에 속해있는지 확인 */
	Client* inviterClient = _clientManager.getClient(sessionIndex);
	std::string inviterNickname = inviterClient->getNickname();
	if (!inviteChannel->isClientInChannel(inviterNickname))
	{
		message._command = "442";
		message._parameters.push_back(inviteChannelName);
		message._trailing = "You're not on that channel";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	/* 초대자(inviter)가 채널 운영자인지 확인 */
	if (!inviteChannel->isOperator(inviterNickname))
	{
		message._command = "482";
		message._parameters.push_back(inviteChannelName);
		message._trailing = "You're not channel operator";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	/* 초대한 클라이언트가 존재하는지 확인 */
	Client* inviteClient = _clientManager.getClientByNickname(inviteNickname);
	if (!inviteClient)
	{
		message._command = "401";
		message._parameters.push_back(inviteNickname);
		message._trailing = "No such nick";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	/* 초대한 클라이언트가 이미 채널에 속해있는지 확인 */
	if (inviteChannel->isClientInChannel(inviteNickname))
	{
		message._command = "443";
		message._parameters.push_back(inviteNickname);
		message._trailing = "is already on channel";
		std::string res = message.toString();
		_sendPacketFunc(sessionIndex, res);
		return ;
	}

	inviteChannel->addInvitedClient(inviteClient->getSessionIndex());

	message._prefix = inviterNickname + "!" + inviterClient->getUsername() + "@" + inviterClient->getServername();
	message._command = "INVITE";
	message._parameters.push_back(inviterNickname);
	message._trailing = inviteChannelName;
	std::string res = message.toString();
	/* 초대한 클라이언트에게 메시지 전송 */
	_sendPacketFunc(inviteClient->getSessionIndex(), res);
	/* 초대자(inviter)에게 메시지 전송*/
	_sendPacketFunc(sessionIndex, res);
}
