#ifndef IRC_SOCKET_REACTOR_HPP
#define IRC_SOCKET_REACTOR_HPP

#include <cstdio>
#include <cstdlib>
#include <sys/event.h>
#include <netinet/in.h>
#include <map>
#include "../socket/ServerSocket.hpp"
#include "../session/Session.hpp"

class Session;

class SocketReactor
{
	public:
		SocketReactor(ServerSocket serverSocket);

		void addSocket(int clientSocket);
		void removeSocket(int clientSocket);
		void accept();
		void run();

	private:
		int _kqueue;

		ServerSocket _serverSocket;
		struct kevent *_currentEvent;
		uintptr_t _currentEventSocket;

		std::map<int, Session *> sessions;


};

#endif
