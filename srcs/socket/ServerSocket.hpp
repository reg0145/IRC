#ifndef SERVER_SOCKET_HPP
#define SERVER_SOCKET_HPP

#include <cerrno>
#include <iostream>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>

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
