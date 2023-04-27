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
		static std::vector<std::string> split(const std::string& str, const std::string& delimiter) 
		{
			std::vector<std::string> tokens;
			std::string::size_type pos = 0;

			while (pos != std::string::npos)
			{
					std::string::size_type end = str.find(delimiter, pos);
					if (end == std::string::npos) 
					{
							tokens.push_back(str.substr(pos));
							break;
					}
					tokens.push_back(str.substr(pos, end - pos));
					pos = end + delimiter.size();
			}

			return tokens;
		}
	
		static IRCMessage parse(std::string &request)
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
			"PASS <password> (optional)\r\n",
			"NICK <nickname>\r\n",
			"USER <username> <hostname> <servername> <realname>\r\n",
			"JOIN <channel>,<channel> <key>\r\n",
			"PRIVMSG <target> <message>\r\n",
			"PART <channel>\r\n",
			"",
			":prefix command param1 param2 :trailing\r\n",
			":prefix command param1 param2 :trailing  with   spaces \r\n",
			":prefix\r\n",
	};

	for(const char *packet : packets){
		std::vector<std::string> msgs = IRCMessageParser::split(packet, "\r\n");
		IRCMessage msg = IRCMessageParser::parse(msgs[0]);
		std::cout << "\"" << packet <<"\"" << std::endl;
		std::cout << "- prefix: " << msg.prefix << std::endl;
		std::cout << "- command: " << msg.command <<  std::endl;
		std::cout << "- parameters[" <<  msg.parameters.size() << "]: [";
		for (std::string param : msg.parameters) {
			std::cout << param << "| ";
		}
		std::cout << "]\n";
		std::cout << "- traling: " << msg.trailing << "|END."<< std::endl;
		std::cout << std::endl;
	}
}
