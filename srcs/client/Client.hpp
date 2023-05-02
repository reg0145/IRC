#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <set>

class Client {
	public:
		Client(int sessionIndex);
		~Client();

		void leaveChannel(std::string chnannelName);
		void leaveClient();

		int getSessionIndex() const;
		std::string getNickname() const;
		std::string getChannel(std::string channelName);
		bool getIsPass() const;
		void setNickname(std::string nickname);
		void setName(std::string name);
		void setHostname(std::string hostname);

	private:
		int _sessionIndex;
		std::string _name;
		std::string _nickname;
		std::string _hostname;
		std::set<std::string> _channels;
		bool _isPass;
		bool _isAdmin;
};


#endif
