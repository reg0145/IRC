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

		void bind(char *strPort);
		void listen();
		int accept();

		bool operator==(int socket) const;

		int getSocket() const;


	private :
		int _socket;
};

#endif
