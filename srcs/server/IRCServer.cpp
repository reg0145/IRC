#include "IRCServer.hpp"

void IRCServer::init(int port, char *password)
{
	(void)password;
	_serverSocket.bind(port);
	_serverSocket.listen();

	_socketReactor.init(*this, &IRCServer::onRequestHandler, &IRCServer::onRequestErrorHandler);
	_socketReactor.addSocket(_serverSocket.getSocket());
}

void IRCServer::start()
{
	while (true)
	{
		_socketReactor.run();
	}
}

void IRCServer::onRequestHandler(int socket)
{
	if (_serverSocket == socket)
	{
		int clientSocket = _serverSocket.accept();

		if (clientSocket == -1)
		{
			return ;
		}

		if (_sessionManager.registerSession(clientSocket) == -1)
		{
			return ;
		}

		_socketReactor.addSocket(clientSocket);
	}
	else
	{
		_sessionManager.getSessionBySocket(socket)->onReadable();
	}
}

void IRCServer::onRequestErrorHandler(int socket)
{
	if (_serverSocket == socket)
	{
		std::cerr << "IRCServer onRequestErrorHandler() : ServerSocket Event Error" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		_sessionManager.unRegisterSessionBySocket(socket);
	}
}

//void IRCServer::process()
//{
//	for(std::deque<RecvPacketInfo>::iterator it = packetQueue.begin(); it != packetQueue.end(); ++it)
//	{
//		_packetManager.process(*it);
//	}
//}
