#include "Session.hpp"

void (*Session::_addPacketFunc)(int, IRCMessage) = 0;

Session::Session(int sessionIndex, int clientSocket) : _sessionIndex(sessionIndex), _clientSocket(clientSocket)
{
}

Session::~Session()
{
	::close(_clientSocket);
}


#include <iostream>
void Session::onReadable()
{
	_messages.clear();

	int received = recv(_clientSocket, _buffer, sizeof(_buffer) - 1, 0);

	if (received < 1)
	{
		close();
	}
	else
	{
		_buffer[received] = 0;
		_messages = IRCMessage::parse(_buffer);
		std::cout << "\"" << _buffer <<"\"" << std::endl;
		for (std::list<IRCMessage>::iterator it = _messages.begin(); it != _messages.end(); it++)
		{
			std::cout << "- prefix: " << it->_prefix << std::endl;
			std::cout << "- command: " << it->_command << std::endl;
			std::cout << "- parameters[" <<  it->_parameters.size() << "]: ";
			for (std::vector<std::string>::iterator iter = it->_parameters.begin(); iter != it->_parameters.end(); ++iter)
				std::cout << *iter << ", ";
			std::cout << "\n";
			std::cout << "- traling: " << it->_trailing << std::endl;
			std::cout << std::endl;
			_addPacketFunc(_sessionIndex, *it);
		}
	}
}

void Session::close()
{
	IRCMessage message;
	message._command = "DISCONNECT";
	_messages.push_back(message);

	_addPacketFunc(_sessionIndex, _messages.front());
}

int Session::getClientSocket()
{
	return _clientSocket;
}

void Session::sendPacket(std::string &res)
{
	send(_clientSocket, res.c_str(), res.size(), 0);
}
