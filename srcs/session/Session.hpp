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

		static void(*_addPacketFunc)(int sessionIndex, IRCMessage message);

	private :
		void close();
		char _tmp[512];
		std::stringstream _buffer;
		std::list<IRCMessage> _messages;
		int _sessionIndex;
		int _clientSocket;
};

#endif
