#include "IRCServer.hpp"

std::vector<Session *> IRCServer::_sessions = std::vector<Session *>(1024);
std::deque<int> IRCServer::_sessionIndexPool = std::deque<int>(1024);

void IRCServer::init(int port, char *password)
{
	(void)password;
	_serverSocket.bind(port);
	_serverSocket.listen();

	// null 가드 필요
	_socketReactor = new SocketReactor(_serverSocket);

	for (int i = 0; i < 1024; ++i)
	{
		_sessionIndexPool.push_back(i);
		_sessions.push_back(0);
	}

	Session::_onConnection = IRCServer::onConnect;
}

void IRCServer::start()
{
	while (true)
	{
		_socketReactor->run();
	}
}

void IRCServer::sendPacket(int sessionIndex, const char* data, const int size)
{
	_sessions[sessionIndex]->sendPacket(data, size);
}

int IRCServer::takeSessionIndex()
{
	if (_sessionIndexPool.empty())
	{
		return -1;
	}

	int sessionIndex = _sessionIndexPool.front();
	_sessionIndexPool.pop_front();

	return sessionIndex;
}

void IRCServer::freeSessionIndex(int sessionIndex)
{
	_sessionIndexPool.push_back(sessionIndex);
}

void IRCServer::onConnect(Session *session)
{
	int sessionIndex = takeSessionIndex();

	if (sessionIndex == -1)
	{
		// 에러 처리
		return ;
	}

	_sessions[sessionIndex] = session;
}