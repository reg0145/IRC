#include "IRCMessage.hpp"

std::list<IRCMessage> IRCMessage::parse(const char* requests)
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

std::list<std::string> IRCMessage::split(const std::string& str, const std::string& delimiter)
{
	std::list<std::string> tokens;
	std::string::size_type pos = 0;

	while (pos != std::string::npos)
	{
			std::string::size_type end = str.find(delimiter, pos);
			if (end == std::string::npos)
			{
				if (str.size() != pos)
				{
					tokens.push_back(str.substr(pos));
				}
				break;
			}
			tokens.push_back(str.substr(pos, end - pos));
			pos = end + delimiter.size();
	}

	return tokens;
}

IRCMessage IRCMessage::parseMessage(const std::string &request)
{
	IRCMessage msg;
	std::istringstream ss(request);
	std::string params;
	std::string token;

	size_t idx = request.find(':', 1);
	if (idx != std::string::npos)
	{
		msg._hasTrailing = true;
	}
	if (request[0] == ':')
	{
		ss.ignore(1);
		std::getline(ss, msg._prefix, ' ');
	}
	std::getline(ss, msg._command, ' ');

	std::getline(ss, params, ':');
	std::istringstream iss(params);
	while (iss >> token)
	{
		msg._parameters.push_back(token);
	}
	std::getline(ss, msg._trailing);

	return msg;
}

std::string IRCMessage::toString()
{
	std::ostringstream oss;

	if (_prefix != "")
	{
		oss << ":" << _prefix << " ";
	}

	oss << _command;

	for (size_t i = 0; i < _parameters.size(); i++)
	{
		oss << " " << _parameters[i];
	}

	if (_trailing != "")
	{
		oss << " :" << _trailing;
	}

	oss << "\r\n";

	return oss.str();
}
