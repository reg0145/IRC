#ifndef IRC_SESSION_HPP
#define IRC_SESSION_HPP

#include <netinet/in.h>
#include <unistd.h>
#include "../reactor/SocketReactor.hpp"

class Session
{
	public :
		Session(int socket);

		void onReadable();
		void sendPacket(const char* data, const int size);

		static void (*_onConnection)(Session *);

	private :
		void close();
		void toPacketProcess(int bufferSize, char *buffer);

		char _buffer[512];
		int _clientSocket;
};

#endif
