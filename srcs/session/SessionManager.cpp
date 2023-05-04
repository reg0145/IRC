#include "SessionManager.hpp"

std::vector<Session*> SessionManager::_sessions = std::vector<Session*>();

SessionManager::SessionManager()
{
	for (int i = 0; i < 1024; ++i)
	{
		_sessionIndexPool.push_back(i);
		_sessions.push_back(0);
	}
}

int SessionManager::registerSession(int clientSocket)
{
	int sessionIndex = takeSessionIndex();

	if (sessionIndex == -1)
	{
		return -1;
	}

	Session* session = new(std::nothrow) Session(sessionIndex, clientSocket);

	if (session == 0)
	{
		return -1;
	}

	_sessionIndexMap[clientSocket] = sessionIndex;
	_sessions[sessionIndex] = session;

	return 0;
}

void SessionManager::unRegisterSession(int sessionIndex)
{
	if (_sessions[sessionIndex]) {
		freeSessionIndex(sessionIndex);
		_sessionIndexMap.erase(_sessions[sessionIndex]->getClientSocket());

		delete _sessions[sessionIndex];
		_sessions[sessionIndex] = 0;
	}
}

void SessionManager::unRegisterSessionBySocket(int clientSocket)
{
	int sessionIndex = _sessionIndexMap[clientSocket];
	unRegisterSession(sessionIndex);
}

int SessionManager::takeSessionIndex()
{
	if (_sessionIndexPool.empty())
	{
		return -1;
	}

	int sessionIndex = _sessionIndexPool.front();
	_sessionIndexPool.pop_front();

	return sessionIndex;
}

void SessionManager::freeSessionIndex(int sessionIndex)
{
	_sessionIndexPool.push_back(sessionIndex);
}

Session* SessionManager::getSessionBySocket(int clientSocket)
{
	int sessionIndex = _sessionIndexMap[clientSocket];
	return _sessions[sessionIndex];
}

void SessionManager::sendPacketFunc(int sessionIndex, std::string &res)
{
	_sessions[sessionIndex]->sendPacket(res);
}

