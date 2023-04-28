#ifndef SOCKETREACTOR_HPP
#define SOCKETREACTOR_HPP

#include <cerrno>
#include <iostream>
#include <sys/event.h>

template <class T>
class SocketReactor
{
	typedef void (T::*EventHandler)(int);

	public:
		void init(T& Object, EventHandler onEventSuccess, EventHandler onEventError);
		void addSocket(int clientSocket);
		void removeSocket(int clientSocket);
		void run();

	private:
		int _kqueue;
		struct kevent *_currentEvent;

		T* _object;
		EventHandler _onEventSuccess;
		EventHandler _onEventError;
};

#include "SocketReactor.tpp"

#endif
