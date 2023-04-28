#ifndef IRCMESSAGEPARSER_HPP
#define IRCMESSAGEPARSER_HPP

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
			std::list<std::string> messages;

			messages = split(requests, "\r\n");
			for (std::list<std::string>::const_iterator it = messages.begin(); it != messages.end(); ++it)
			{
				IRCMessages.push_back(parseMessage(*it));
			}

			return IRCMessages;
		}
	
		static std::list<std::string> split(const std::string& str, const std::string& delimiter)
		{
			std::list<std::string> tokens;
			std::string::size_type pos = 0;

			while (pos != std::string::npos)
			{
					std::string::size_type end = str.find(delimiter, pos);
					if (end == std::string::npos)
					{
						break;
					}
					tokens.push_back(str.substr(pos, end - pos));
					pos = end + delimiter.size();
			}

			return tokens;
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
