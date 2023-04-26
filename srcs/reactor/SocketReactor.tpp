#include "SocketReactor.hpp"

template <typename T>
void SocketReactor<T>::init(T& object, EventHandler onEventSuccess, EventHandler onEventError)
{
	if((_kqueue = kqueue()) < 0)
	{
		std::cerr << "SocketReactor init() : " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	_object = &object;
	_onEventSuccess = onEventSuccess;
	_onEventError = onEventError;
}

template <typename T>
void SocketReactor<T>::addSocket(int clientSocket)
{
	struct kevent event;
	EV_SET(&event, clientSocket, EVFILT_READ, EV_ADD, 0, 0, 0);
	kevent(_kqueue, &event, 1, 0, 0, 0);
}

template <typename T>
void SocketReactor<T>::removeSocket(int clientSocket)
{
	struct kevent event;
	EV_SET(&event, clientSocket, EVFILT_READ, EV_DELETE, 0, 0, 0);
	kevent(_kqueue, &event, 1, 0, 0, 0);
}

template <typename T>
void SocketReactor<T>::run()
{
	const int MAX_EVENT = 100;

	struct kevent events[MAX_EVENT];
	int nEvents = kevent(_kqueue, 0, 0, events, MAX_EVENT, 0);

	for (int i = 0; i < nEvents; ++i)
	{
		_currentEvent = &events[i];

		if (_currentEvent->flags & EV_ERROR)
		{
			(_object->*_onEventError)(_currentEvent->ident);
		}
		if (_currentEvent->flags & EV_EOF)
		{
			removeSocket(_currentEvent->ident);
		}
		if (_currentEvent->filter == EVFILT_READ)
		{
			(_object->*_onEventSuccess)(_currentEvent->ident);
		}
	}
}
