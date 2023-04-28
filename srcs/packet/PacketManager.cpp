#include "PacketManager.hpp"
#include <iostream>

void PacketManager::init()
{
	_recvFuntionDictionary["DISCONNECT"] = &PacketManager::processDisconnect;
	_recvFuntionDictionary["NICK"] = &PacketManager::processNick;
	_recvFuntionDictionary["PASS"] = &PacketManager::processPass;
	_recvFuntionDictionary["USER"] = &PacketManager::processUser;
}

void PacketManager::process(int clientSocket, int sessionIndex, IRCMessage &message)
{
	std::map<std::string, PROCESS_RECV_PACKET_FUNCTION>::iterator it;
  it = _recvFuntionDictionary.find(message.command);

	if (it == _recvFuntionDictionary.end())
	{
		return ;
	}

	(this->*(it->second))(clientSocket, sessionIndex, message);
}

void PacketManager::processDisconnect(int clientSocket, int sessionIndex, IRCMessage &message)
{
	(void)message;
	(void)clientSocket;
	std::cout << ">> client[" << sessionIndex << "] Disconnected <<" << std::endl;
}

void PacketManager::processPass(int clientSocket, int sessionIndex, IRCMessage &message)
{
	(void)sessionIndex;
	(void)message;
	std::string res = "001 : process Pass Perfect!\r\n";
	send(clientSocket, res.c_str(), res.size(), 0);
}

void PacketManager::processNick(int clientSocket, int sessionIndex, IRCMessage &message)
{
	(void)clientSocket;
	Client* client = _clientManager.getClient(sessionIndex);
	
	if (!client->getIsPass())
	{
		return ;
	}
	
	/* 파라미터 1개 이상 존재하는가?*/
	if (message.parameters.size() != 1)
	{
		//sendPacketFunc();
		return ;
	}
	
	/* 닉네임 중복여부 확인 */
	std::string &nickname = message.parameters[0];
	if (_clientManager.checkNick(nickname))
	{
		//sendPacketFunc();
		return ;
	}
	
	client->setNickname(nickname);
	
	_clientManager.addClient(sessionIndex, nickname);
	//sendPacketFunc();
}

void PacketManager::processUser(int clientSocket, int sessionIndex, IRCMessage &message)
{
	(void)clientSocket;
	if (_clientManager.checkClient(sessionIndex))
	{
		//sendPacketFunc();
		return ;
	}

	/* 파라미터 1개 이상 존재하는가?*/
	if (message.parameters.size() != 3)
	{
		//sendPacketFunc();
		return ;
	}
	
	std::string &nickname = message.parameters[0];
	if (_clientManager.checkNick(nickname))
	{
		//sendPacketFunc();
		return ;
	}

	
	Client* client = _clientManager.getClient(sessionIndex);
	std::string &hostname = message.parameters[1];
	std::string &name = message.parameters[2];
	client->setNickname(nickname);
	client->setName(hostname);
	client->setHostname(name);
	
	//sendPacketFunc();
}


