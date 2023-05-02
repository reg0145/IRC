#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <set>

class Client {
	public:
		Client(int sessionIndex);
		~Client();

		void addChannel(std::string channelName);
		void removeChannel(std::string chnannelName);
		void leaveClient();

		int getSessionIndex() const;
		std::string getName() const;
		std::string getNickname() const;
		std::string getHostname() const;
		std::string getChannel(std::string channelName);

		bool getIsPass() const;
		void setPassTrue();
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
