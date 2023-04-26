#ifndef IRC_IRCMESSAGEPARSER_HPP
#define IRC_IRCMESSAGEPARSER_HPP

#include <list>
#include <sstream>
#include <iostream>
#include "IRCMessage.hpp"

class IRCMessageParser
{
	public:
		static std::list<IRCMessage> parse(const char* requests)
		{
			std::list<IRCMessage> IRCMessages;
			std::string request;

			std::istringstream iss(requests);

			while(std::getline(iss, request, '\n'))
			{
				IRCMessages.push_back(parseMessage(request));
			}

			return IRCMessages;
		}

	private:
		static IRCMessage parseMessage(const std::string &request)
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

#endif
