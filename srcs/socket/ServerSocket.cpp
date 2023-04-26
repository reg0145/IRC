#include "ServerSocket.hpp"

ServerSocket::ServerSocket()
{
	if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "ServerSocket socket() : " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	/* SO_REUSEADDR 옵션 설정 */
	int option = 1;
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0)
	{
		std::cerr << "ServerSocket setsockopt() : " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	/* NONBLOCK 플래그 설정 */
	if (fcntl(_socket, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "ServerSocket fcntl() : " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
}

void ServerSocket::bind(int port)
{
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(port);

	/* 포트 바인딩 */
	if (::bind(_socket, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in)) < 0)
	{
		std::cerr << "ServerSocket bind() : " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
}

void ServerSocket::listen()
{
	if (::listen(_socket, 10) < 0)
	{
		std::cerr << "ServerSocket listen() : " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
}

int ServerSocket::accept()
{
	int clientSocket = ::accept(_socket, 0, 0);

	if (clientSocket < 0)
	{
		std::cerr << "ServerSocket accept() : " << errno << std::endl;
		return -1;
	}

	return clientSocket;
}

bool ServerSocket::operator==(int socket) const
{
	return _socket == socket;
}

int ServerSocket::getSocket() const
{
	return (_socket);
}
