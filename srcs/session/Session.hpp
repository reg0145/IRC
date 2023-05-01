#ifndef SESSION_HPP
#define SESSION_HPP

#include <list>
#include <deque>
#include <unistd.h>
#include <netinet/in.h>
#include "../message/IRCMessage.hpp"
#include "../packet/PacketManager.hpp"

class Session
{
	public :
		Session(int sessionIndex, int clientSocket);
		~Session();

		void onReadable();
		void sendPacket(std::string&);

		int getClientSocket();

		static void(*_addPacketFunc)(int, IRCMessage&);

	private :
		void close();

		char _buffer[512];
		std::list<IRCMessage> _messages;
		int _sessionIndex;
		int _clientSocket;
};

#endif
