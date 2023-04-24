#include "Session.hpp"

void (*Session::_onConnection)(Session *) = 0;

Session::Session(int clientSocket) : _clientSocket(clientSocket)
{
	_onConnection(this);
}

void Session::onReadable()
{
	int received;

	received = recv(_clientSocket, _buffer, sizeof(_buffer), 0);
	if (received == -1)
	{
		close();
	}
	else
	{
		toPacketProcess(received, _buffer);
	}
}

void Session::toPacketProcess(int bufferSize, char *buffer)
{
	const int MAX_PACKET_SIZE = 512;

	/* Raw 패킷 유효성 검사 */
	if (bufferSize > MAX_PACKET_SIZE)
	{
		/* 메시지 크기 초과 에러 보내기 */
		return ;
	}

	(void)buffer;
}

void Session::close()
{
	::close(_clientSocket);
	/* Reactor 이벤트 해제 필요*/
}

void Session::sendPacket(const char* data, const int size)
{
	/* send() 예외 처리 필요 */
	send(_clientSocket, data, size, 0);
}