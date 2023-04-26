#ifndef IRC_SERVER_HPP
#define IRC_SERVER_HPP

#include <deque>
#include <vector>
#include <iostream>
#include <cerrno>
#include "../session/Session.hpp"
#include "../session/SessionManager.hpp"
#include "../reactor/SocketReactor.hpp"
#include "../socket/ServerSocket.hpp"

class IRCServer
{
	public:
		void init(int port, char *password);
		void start();

	private:
		void onRequestHandler(int socket);
		void onRequestErrorHandler(int socket);

		int registerSession(int sessionIndex);
		void unRegisterSession(int sessionIndex);

		int takeSessionIndex();
		void freeSessionIndex(int sessionIndex);

		ServerSocket _serverSocket;
		SocketReactor<IRCServer> _socketReactor;
		SessionManager _sessionManager;
		std::deque<int> _packetQueue;
};

#endif