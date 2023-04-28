#ifndef IRCMESSAGE_HPP
#define IRCMESSAGE_HPP

#include <string>
#include <vector>

struct IRCMessage
{
	std::string prefix;
	std::string command;
	std::vector<std::string> parameters;
	std::string trailing;
};

#endif
