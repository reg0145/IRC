#ifndef IRC_SESSION_HPP
#define IRC_SESSION_HPP

#include <list>
#include <deque>
#include <unistd.h>
#include <netinet/in.h>
#include "../message/IRCMessage.hpp"
#include "../message/IRCMessageParser.hpp"
#include "../packet/PacketManager.hpp"

class Session
{
	public :
		Session(int sessionIndex, int clientSocket);

		void onReadable();

		static std::deque<RecvPacketInfo>* packetQueue;

	private :
		void close();

		char _buffer[512];
		std::list<IRCMessage> _messages;
		int _sessionIndex;
		int _clientSocket;
		PacketManager _packetManager;
};

#endif
