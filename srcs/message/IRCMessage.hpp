#ifndef IRCMESSAGE_HPP
#define IRCMESSAGE_HPP

#include <list>
#include <string>
#include <vector>
#include <sstream>

class IRCMessage
{
	public:
		static std::list<IRCMessage> parse(const std::string& requests);
		static std::list<std::string> split(const std::string& str, const std::string& delimiter);

		std::string toString();
		void clear();

		std::string _prefix;
		std::string _command;
		std::vector<std::string> _parameters;
		std::string _trailing;
		bool _hasTrailing;

	private:
		static IRCMessage parseMessage(const std::string &request);
};

#endif
