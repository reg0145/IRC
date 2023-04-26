#ifndef IRC_SESSION_HPP
#define IRC_SESSION_HPP

#include <unistd.h>
#include <netinet/in.h>

class Session
{
	public :
		Session(int sessionIndex, int clientSocket);

		void onReadable();

	private :
		void close();

		char _buffer[512];
		int _sessionIndex;
		int _clientSocket;
};

#endif
