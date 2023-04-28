#ifndef SESSIONMANAGER_HPP
#define SESSIONMANAGER_HPP

#include <map>
#include <deque>
#include <vector>
#include "Session.hpp"

class SessionManager {
	public:
		SessionManager();

		int registerSession(int sessionIndex);
		void unRegisterSession(int sessionIndex);
		void unRegisterSessionBySocket(int clientSocket);

		int takeSessionIndex();
		void freeSessionIndex(int sessionIndex);

		Session*  getSessionBySocket(int clientSocket);

	private:
		std::map<int, int> _sessionIndexMap;
		std::vector<Session*> _sessions;
		std::deque<int> _sessionIndexPool;
};

#endif
