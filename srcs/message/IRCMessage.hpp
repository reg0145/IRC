#ifndef IRC_MESSAGE_HPP
#define IRC_MESSAGE_HPP

#include <string>
#include <vector>

struct IRCMessage
{
	std::string command;
	std::vector<std::string> parameters;
	std::string trailing;
};

#endif
