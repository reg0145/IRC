#ifndef IRC_SERVER_HPP
#define IRC_SERVER_HPP

#include <sys/socket.h>
#include <sys/event.h>
#include <deque>
#include <vector>
#include "../session/Session.hpp"
#include "../reactor/SocketReactor.hpp"
#include "../socket/ServerSocket.hpp"

class IRCServer
{
	public:
		void init(int port, char *password);
		void start();

	private:
		void setFunction();

		void sendPacket(int sessionIndex, const char* data, const int size);

		static int takeSessionIndex();
		static void freeSessionIndex(int sessionIndex);
		static void onConnect(Session *session);

		ServerSocket _serverSocket;
		SocketReactor *_socketReactor;
		static std::vector<Session *> _sessions;
		static std::deque<int> _sessionIndexPool;
		std::deque<int> _packetQueue;
};

#endif