#include "Session.hpp"

Session::Session(int sessionIndex, int clientSocket) : _sessionIndex(sessionIndex), _clientSocket(clientSocket)
{
	_packetManager.init();
}

#include <iostream>
void Session::onReadable()
{
	int received = recv(_clientSocket, _buffer, sizeof(_buffer) - 1, 0);

	if (received < 1)
	{
		close();
	}
	else
	{
		_buffer[received] = 0;
		_messages.clear();
		_messages = IRCMessageParser::parse(_buffer);
		std::cout << "\"" << _buffer <<"\"" << std::endl;
		for (std::list<IRCMessage>::iterator it = _messages.begin(); it != _messages.end(); it++)
		{
			std::cout << "- command: " << it->command << std::endl;
			std::cout << "- parameters[" <<  it->parameters.size() << "]: ";
			for (std::vector<std::string>::iterator iter = it->parameters.begin(); iter != it->parameters.end(); ++iter)
				std::cout << *iter << " ";
			std::cout << "\n";
			std::cout << "- traling: " << it->trailing << std::endl;
			std::cout << std::endl;
			_packetManager.process(_sessionIndex, *it);
		}

	}
}

void Session::close(){
	std::cout << ">> client[" << _sessionIndex << "] Disconnected <<" << std::endl;
	::close(_clientSocket);
	/* Reactor 이벤트 해제 필요*/
}
