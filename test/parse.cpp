#include <sstream>
#include <iostream>
#include <map>
#include <vector>

struct IRCMessage {
	std::string prefix;
	std::string command;
	std::vector<std::string> parameters;
	std::string trailing;
};

class IRCMessageParser {
	public:
		static IRCMessage parse(const char *packet)
		{
			std::istringstream iss(packet);
			std::string token;
			IRCMessage msg;

			if (packet[0] == ':') {
				iss >> token;
				msg.prefix = token.substr(1);
			}

			std::getline(iss, msg.command, ' ');

			while (iss >> token) {
				if (token[0] == ':') {
					msg.trailing = token.substr(1);
					break;
				}
				msg.parameters.push_back(token);
			}

			return msg;
		}
};


int main()
{
	const char *packets[] = {
			"PASS <password> (optional)",
			"NICK <nickname>",
			"USER <username> <hostname> <servername> <realname>",
			"JOIN <channel>",
			"PRIVMSG <target> <message>",
			"PART <channel>",
			"",
			":prefix command param1 param2 :trailing",
			":prefix command param1 param2 :trailing with spaces",
	};

	for(const char *packet : packets){
		IRCMessage msg = IRCMessageParser::parse(packet);
		std::cout << "\"" << packet <<"\"" << std::endl;
		std::cout << "- prefix: " << msg.prefix << std::endl;
		std::cout << "- command: " << msg.command << std::endl;
		std::cout << "- parameters[" <<  msg.parameters.size() << "]: ";
		for (std::string param : msg.parameters) {
			std::cout << param << " ";
		}
		std::cout << "\n";
		std::cout << "- traling: " << msg.trailing << std::endl;
		std::cout << std::endl;
	}
}