#include "PacketManager.hpp"
#include <iostream>

void PacketManager::init()
{
	_recvFuntionDictionary["NICK"] = &PacketManager::processNick;
}

void PacketManager::process(int sessionIndex, IRCMessage &message)
{
	std::map<std::string, PROCESS_RECV_PACKET_FUNCTION>::iterator it;
    it = _recvFuntionDictionary.find(message.command);

	if (it == _recvFuntionDictionary.end())
	{
		return ;
	}

	(this->*(it->second))(sessionIndex, message);
}


void PacketManager::processNick(int sessionIndex, IRCMessage &message)
{
	std::cout << "Nick - sessionIndex = " << sessionIndex << ", command = " << message.command << std::endl;
}

//void PacketManager::process(RecvPacketInfo &packetInfo)
//{
//	std::map<std::string, PROCESS_RECV_PACKET_FUNCTION>::iterator it;
//    it = _recvFuntionDictionary.find(packetInfo.message.command);

//	if (it == _recvFuntionDictionary.end())
//	{
//		return ;
//	}

//	(this->*(it->second))(packetInfo.sessionIndex, packetInfo.message);
//}
