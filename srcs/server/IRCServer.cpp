#include "IRCServer.hpp"

std::deque<RecvPacketInfo> IRCServer::_packetQueue = std::deque<RecvPacketInfo>();

void IRCServer::init(char *port, char *password)
{
	_serverSocket.bind(port);
	_serverSocket.listen();

	_packetManager.init(password);
	_socketReactor.init(*this, &IRCServer::onRequestHandler, &IRCServer::onRequestErrorHandler);
	_socketReactor.addSocket(_serverSocket.getSocket());

	Session::_addPacketFunc = &IRCServer::addPacketFunc;
	PacketManager::_sendPacketFunc = &SessionManager::sendPacketFunc;
}

void IRCServer::start()
{
	while (true)
	{
		_socketReactor.run();
		process();
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

		if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0)
		{
			std::cerr << "ClientSocket fcntl() : " << strerror(errno) << std::endl;
			close(clientSocket);
			return ;
		}

		if (_sessionManager.registerSession(clientSocket) == -1)
		{
			close(clientSocket);
			return ;
		}

		_socketReactor.addSocket(clientSocket);
	}
	else
	{
		Session* session = _sessionManager.getSessionBySocket(socket);

		if (session != NULL)
		{
			session->onReadable();
		}
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
		Session* session = _sessionManager.getSessionBySocket(socket);

		if (session)
		{
			IRCMessage message;
			message._command = "QUIT";

			_packetManager.process(session->getSessionIndex(), message);
			_sessionManager.unRegisterSessionBySocket(socket);
		}
		else
		{
			close(socket);
		}
	}
}

void IRCServer::process()
{
	for (std::deque<RecvPacketInfo>::iterator it = _packetQueue.begin(); it != _packetQueue.end(); ++it)
	{
		_packetManager.process(it->sessionIndex, it->message);

		if (it->message._command == "QUIT")
		{
			_sessionManager.unRegisterSession(it->sessionIndex);
		}
	}
	_packetQueue.clear();
}

void IRCServer::addPacketFunc(int sessionIndex, IRCMessage message)
{
	RecvPacketInfo packetInfo = {sessionIndex, message};
	_packetQueue.push_back(packetInfo);
}

