#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <set>

class Client
{
public:
	Client(int sessionIndex);
	~Client();

	void addChannel(std::string channelName);
	void removeChannel(std::string chnannelName);
	void leaveClient();

	int getSessionIndex() const;
	std::string getName() const;
	std::string getNickname() const;
	std::string getUsername() const;
	std::string getHostname() const;
	std::string getServername() const;
	std::string getChannel(std::string channelName);
	const std::set<std::string> &getChannels() const;

	bool getIsPass() const;
	void setPassTrue();
	void setName(std::string name);
	void setNickname(std::string nickname);
	void setUsername(std::string username);
	void setHostname(std::string hostname);
	void setServername(std::string serversName);

private:
	int _sessionIndex;
	std::string _name;
	std::string _nickname;
	std::string _username;
	std::string _hostname;
	std::string _servername;
	std::set<std::string> _channels;
	bool _isPass;
	bool _isAdmin;
};

#endif
