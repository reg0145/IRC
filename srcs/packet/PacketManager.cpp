#include "PacketManager.hpp"
#include <iostream>

void PacketManager::init()
{
	_recvFuntionDictionary["DISCONNECT"] = &PacketManager::processDisconnect;
	_recvFuntionDictionary["NICK"] = &PacketManager::processNick;
	_recvFuntionDictionary["PASS"] = &PacketManager::processPass;
	_recvFuntionDictionary["USER"] = &PacketManager::processUser;
	_recvFuntionDictionary["PING"] = &PacketManager::processPing;
}

void PacketManager::process(int clientSocket, int sessionIndex, IRCMessage &message)
{
	std::map<std::string, PROCESS_RECV_PACKET_FUNCTION>::iterator it;
	it = _recvFuntionDictionary.find(message._command);

	if (it == _recvFuntionDictionary.end())
	{
		return ;
	}

	(this->*(it->second))(clientSocket, sessionIndex, message);
}

void PacketManager::processDisconnect(int clientSocket, int sessionIndex, IRCMessage &req)
{
	(void)req;
	(void)clientSocket;
	std::cout << ">> client[" << sessionIndex << "] Disconnected <<" << std::endl;
}

void PacketManager::processPass(int clientSocket, int sessionIndex, IRCMessage &req)
{
	(void)sessionIndex;
	(void)req;

	IRCMessage message;
	message._command = "001";
	message._trailing = "process pass!";

	std::string res = message.toString();
	send(clientSocket, res.c_str(), res.size(), 0);
}

void PacketManager::processNick(int clientSocket, int sessionIndex, IRCMessage &req)
{
	(void)clientSocket;
	Client* client = _clientManager.getClient(sessionIndex);

	if (!client->getIsPass())
	{
		return ;
	}

	/* 파라미터 1개 이상 존재하는가?*/
	if (req._parameters.size() != 1)
	{
		//sendPacketFunc();
		return ;
	}

	/* 닉네임 중복여부 확인 */
	std::string &nickname = req._parameters[0];
	if (_clientManager.checkNick(nickname))
	{
		//sendPacketFunc();
		return ;
	}

	client->setNickname(nickname);

	_clientManager.addClient(sessionIndex, nickname);
}

void PacketManager::processUser(int clientSocket, int sessionIndex, IRCMessage &req)
{
	(void)clientSocket;
	(void)req;
	if (_clientManager.checkClient(sessionIndex))
	{
		//sendPacketFunc();
		return ;
	}

	/* 파라미터 1개 이상 존재하는가?*/
	if (req._parameters.size() != 3)
	{
		//sendPacketFunc();
		return ;
	}

	std::string &nickname = req._parameters[0];
	if (_clientManager.checkNick(nickname))
	{
		//sendPacketFunc();
		return ;
	}


	Client* client = _clientManager.getClient(sessionIndex);
	std::string &hostname = req._parameters[1];
	std::string &name = req._parameters[2];
	client->setNickname(nickname);
	client->setName(hostname);
	client->setHostname(name);
}

void PacketManager::processPing(int clientSocket, int sessionIndex, IRCMessage &req)
{
	(void)sessionIndex;
	(void)req;

	if (req._parameters.size() != 1)
	{
		return ;
	}

	IRCMessage message;
	message._command = "PONG";
	message._parameters.push_back(req._parameters[0]);
	message._trailing = ":process pass!";

	std::string res = message.toString();
	send(clientSocket, res.c_str(), res.size(), 0);
}
