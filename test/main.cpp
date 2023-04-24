#include <netinet/in.h>
#include <cstdio>
#include <iostream>
#include <list>
#include <fcntl.h>

int main () {
	int serverSocket;

/* 소켓 생성 */
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		perror("Failed to create server socket");
		return (1);
	}
///* 소켓 옵션 추가 */
	int optval = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	int flags = fcntl(serverSocket, F_GETFL, 0);
	fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK);

/* 소켓 포트 구조체 생성 */
	struct sockaddr_in socketAddr;
	socketAddr.sin_family = AF_INET;
	socketAddr.sin_addr.s_addr = INADDR_ANY;
	socketAddr.sin_port = htons(6697);

/* 소켓 포트 바인딩 */
	if (bind(serverSocket, (struct sockaddr *) &socketAddr, sizeof(struct sockaddr_in)) < 0) {
		perror("Failed to bind())");
		return (1);
	}

/* 소켓 리스닝 모드 */
	if (listen(serverSocket, 10) < 0) {
		perror("Failed to listen()");
		return (1);
	}


///* 클라이언트 연결 대기 */
	int clientSocket;
	std::list<int> clients;
	while (true) {

		clientSocket = accept(serverSocket, 0, 0);
		if (clientSocket > 0) {
			clients.push_back(clientSocket);
		}

		/* 클라이언트 수만큼 반복하여 읽는다. */
		for (std::list<int>::iterator it = clients.begin(); it != clients.end(); it++) {
			char buffer[1024];
			int received;
			received = recv(*it, buffer, 1024, 0);
			if (received > 0) {
				std::cout << "[client (" << *it << ")] " << buffer << std::endl;
				send(*it, buffer, received, 0);
			}
		}
	}
}