#include "Client.hpp"

Client::Client(int sessionIndex)
: _sessionIndex(sessionIndex),
  _name(""),
  _nickname(""),
  _hostname(""),
  _isPass(false),
  _isAdmin(false)
{
	(void)_sessionIndex;
}

Client::~Client()
{
}

void Client::addChannel(std::string channelName)
{
	_channels.insert(channelName);
}

void Client::leaveChannel(std::string channelName)
{
	_channels.erase(channelName);
}

void Client::leaveClient()
{
	_name = "";
	_nickname = "";
	_hostname = "";
	_isPass = false;
	_isAdmin = false;
	_channels.clear();
}

int Client::getSessionIndex() const
{
	return _sessionIndex;
}


std::string Client::getNickname() const
{
	return _nickname;
}

std::string Client::getChannel(std::string channelName)
{
	std::set<std::string>::iterator it =  _channels.find(channelName);
	if (it == _channels.end())
	{
		return "";
	}

	return *it;
}

bool Client::getIsPass() const
{
	return _isPass;
}

void	Client::setNickname(std::string nickname)
{
	_nickname = nickname;
}

void	Client::setName(std::string name)
{
	_name = name;
}

void	Client::setHostname(std::string hostname)
{
	_hostname = hostname;
}
