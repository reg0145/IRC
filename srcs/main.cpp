#include "server/IRCServer.hpp"

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		return (1);
	}
	IRCServer server;

	server.init(8080, argv[2]);
	server.start();
}
