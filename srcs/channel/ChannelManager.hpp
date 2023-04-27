#ifndef IRC_CHANNELMANAGER_HPP
#define IRC_CHANNELMANAGER_HPP

#include <map>
#include <string>

class ChannelManager {
	public:
		ChannelManager();
		~ChannelManager();

		void checkChannel(std::string channelName);
		void addChannel(std::string channelName);
		void removeChannel(std::string channelName);
		Channel* getChannel(std::string channelName);

	private:
		std::map<std::string, Channel*> _channels;
};

#endif