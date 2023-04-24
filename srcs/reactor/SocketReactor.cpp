#include "SocketReactor.hpp"

SocketReactor::SocketReactor(ServerSocket serverSocket) : _serverSocket(serverSocket), _currentEvent(0), _currentEventSocket(0)
{
	if((_kqueue = kqueue()) < 0)
	{
		perror("SocketReactor : Failed to kqueue()");
		exit(EXIT_FAILURE);
	}

	addSocket(serverSocket.getSocket());
}

/* kqueue 이벤트를 등록합니다.*/
void SocketReactor::addSocket(int clientSocket)
{
	struct kevent event;
	EV_SET(&event, clientSocket, EVFILT_READ, EV_ADD, 0, 0, 0);
	kevent(_kqueue, &event, 1, 0, 0, 0);
}

/* kqueue 이벤트를 삭제합니다.*/
void SocketReactor::removeSocket(int clientSocket)
{
	struct kevent event;
	EV_SET(&event, clientSocket, EVFILT_READ, EV_DELETE, 0, 0, 0);
	kevent(_kqueue, &event, 1, 0, 0, 0);
}

/* kqueue 이벤트가 발생했는지 확인합니다.*/
void SocketReactor::run()
{
	const int MAX_EVENT = 100;

	struct kevent events[MAX_EVENT];
	int nEvents = kevent(_kqueue, 0, 0, events, MAX_EVENT, 0);

	for (int i = 0; i < nEvents; ++i)
	{
		_currentEvent = &events[i];
		_currentEventSocket = events[i].ident;

		if (_currentEvent->flags & EV_ERROR)
		{
			if(_serverSocket == _currentEventSocket)
			{
				/* serverSocket Error 예외 */
			}
			else
			{
				/* clientSocket Error 예외 */
			}
		}

		if (_currentEvent->filter == EVFILT_READ)
		{
			if (_serverSocket == _currentEventSocket)
			{
				accept();
			}
			else
			{
				sessions[_currentEventSocket]->onReadable();
			}
		}
	}
}

void SocketReactor::accept()
{
	int clientSocket;

	if ((clientSocket = _serverSocket.accept()) == -1)
	{
		return ;
	}

	Session *session = new Session(clientSocket);
	if (session == 0)
	{
		perror("SocketReactor : Failed to create EventHandler()");
		return ;
	}

	sessions[clientSocket] = session;

	addSocket(clientSocket);
}
