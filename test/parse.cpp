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
		static IRCMessage parse(const char *request)
		{
			IRCMessage msg;
			std::istringstream ss(request);
			std::string params;
			std::string token;


			if (request[0] == ':')
			{
				ss.ignore(1);
				std::getline(ss, msg.prefix, ' ');
			}

			std::getline(ss, msg.command, ' ');

			std::getline(ss, params, ':');
			std::istringstream iss(params);
			while (iss >> token)
			{
				msg.parameters.push_back(token);
			}

			std::getline(ss, msg.trailing);

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
			":prefix command param1 param2 :trailing  with   spaces ",
			":prefix\r\n",
	};

	for(const char *packet : packets){
		IRCMessage msg = IRCMessageParser::parse(packet);
		std::cout << "\"" << packet <<"\"" << std::endl;
		std::cout << "- prefix: " << msg.prefix << std::endl;
		std::cout << "- command: " << msg.command <<  std::endl;
		std::cout << "- parameters[" <<  msg.parameters.size() << "]: ";
		for (std::string param : msg.parameters) {
			std::cout << param << " ";
		}
		std::cout << "\n";
		std::cout << "- traling: " << msg.trailing << "\\*"<< std::endl;
		std::cout << std::endl;
	}
}
