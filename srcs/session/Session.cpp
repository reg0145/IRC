#include "Session.hpp"

Session::Session(int sessionIndex, int clientSocket) : _sessionIndex(sessionIndex), _clientSocket(clientSocket)
{
}

#include <iostream>
void Session::onReadable()
{
	int received = recv(_clientSocket, _buffer, sizeof(_buffer) - 1, 0);

	if (received < 1)
	{
		std::cout << ">> client[" << _sessionIndex << "] Disconnected <<" << std::endl;
		close();
	}
	else
	{
		std::cout << "client[" << _sessionIndex << "] : " << _buffer << std::endl;
		_buffer[received] = 0;
		send(_clientSocket, _buffer, received, 0);
	}
}

void Session::close()
{
	::close(_clientSocket);
	/* Reactor 이벤트 해제 필요*/
}
