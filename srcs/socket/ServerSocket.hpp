#ifndef SERVER_SOCKET_HPP
#define SERVER_SOCKET_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>

class ServerSocket
{
	public :
		ServerSocket();

		int getSocket() const;

		void bind(int port);
		void listen();
		int accept();

		bool operator==(int socket) const;

	private :
		int _socket;
};

#endif
