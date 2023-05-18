#ifndef IRCSERVER_HPP
#define IRCSERVER_HPP

#include <deque>
#include <vector>
#include <iostream>
#include <cerrno>
#include "../session/Session.hpp"
#include "../session/SessionManager.hpp"
#include "../reactor/SocketReactor.hpp"
#include "../socket/ServerSocket.hpp"
#include "../packet/PacketManager.hpp"

class IRCServer
{
	public:
		void init(char *port, char* password);
		void start();

		static void addPacketFunc(int sessionIndex, IRCMessage message);

	private:
		void onRequestHandler(int socket);
		void onRequestErrorHandler(int socket);
		void process();

		ServerSocket _serverSocket;
		SocketReactor<IRCServer> _socketReactor;
		SessionManager _sessionManager;
		PacketManager _packetManager;

		static std::deque<RecvPacketInfo> _packetQueue;
};

#endif
