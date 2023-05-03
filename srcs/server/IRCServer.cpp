#include "IRCServer.hpp"

std::deque<RecvPacketInfo> IRCServer::_packetQueue = std::deque<RecvPacketInfo>();
std::deque<int> IRCServer::_disconnectPacketQueue = std::deque<int>();

void IRCServer::init(int port, char *password)
{

	_serverSocket.bind(port);
	_serverSocket.listen();

	_packetManager.init(password);
	_socketReactor.init(*this, &IRCServer::onRequestHandler, &IRCServer::onRequestErrorHandler);
	_socketReactor.addSocket(_serverSocket.getSocket());

	Session::_addPacketFunc = &IRCServer::addPacketFunc;
	PacketManager::_sendPacketFunc = &SessionManager::sendPacketFunc;
	PacketManager::_addDisConnectPacketFunc = &IRCServer::addDisConnectPacketFunc;
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

void IRCServer::process()
{
	for (std::deque<RecvPacketInfo>::iterator it = _packetQueue.begin(); it != _packetQueue.end(); ++it)
	{
		_packetManager.process(it->sessionIndex, it->message);
	}
	for (std::deque<int>::iterator it = _disconnectPacketQueue.begin(); it != _disconnectPacketQueue.end(); ++it)
	{
		const IRCMessage message = {0,"DISCONNECT", std::vector<std::string>(0), 0};
		_packetManager.process(*it, const_cast<IRCMessage&>(message));
		_sessionManager.unRegisterSession(*it);
	}

	_packetQueue.clear();
	_disconnectPacketQueue.clear();
}

void IRCServer::addPacketFunc(int sessionIndex, IRCMessage message)
{
	RecvPacketInfo packetInfo = {sessionIndex, message};
	_packetQueue.push_back(packetInfo);
}

void IRCServer::addDisConnectPacketFunc(int sessionIndex)
{
	_disconnectPacketQueue.push_back(sessionIndex);
}

