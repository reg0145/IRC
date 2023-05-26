#include "Session.hpp"

void (*Session::_addPacketFunc)(int sessionIndex, IRCMessage message) = 0;

Session::Session(int sessionIndex, int clientSocket) : _sessionIndex(sessionIndex), _clientSocket(clientSocket)
{
}

Session::~Session()
{
	::close(_clientSocket);
}

void Session::onReadable()
{
	int received = recv(_clientSocket, _tmp, sizeof(_tmp) - 1, 0);

	if (received < 1)
	{
		close();
		return ;
	}

	_messages.clear();
	_tmp[received] = 0;
	std::string str(_tmp);
	std::string::size_type idx = str.rfind("\r\n");

	if (idx == std::string::npos)
	{
		_buffer << _tmp;
		return ;
	}

	_messages = IRCMessage::parse(_buffer.str() + str.substr(0, idx + 2));
	_buffer.str(str.substr(idx + 2));

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
		if (it->_command == "QUIT")
		{
			break;
		}
	}
}

void Session::close()
{
	IRCMessage message;
	message._command = "QUIT";

	_addPacketFunc(_sessionIndex, message);
}

int Session::getClientSocket()
{
	return _clientSocket;
}

int Session::getSessionIndex()
{
	return _sessionIndex;
}

void Session::sendPacket(std::string &res)
{
	send(_clientSocket, res.c_str(), res.size(), 0);
}
