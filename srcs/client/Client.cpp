#include "Client.hpp"

Client::Client(int sessionIndex)
	: _sessionIndex(sessionIndex),
	  _name(""),
	  _nickname(""),
	  _username(""),
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

void Client::removeChannel(std::string channelName)
{
	_channels.erase(channelName);
}

void Client::leaveClient()
{
	_name = "";
	_nickname = "";
	_username = "";
	_hostname = "";
	_servername = "";
	_isPass = false;
	_isAdmin = false;
	_channels.clear();
}

int Client::getSessionIndex() const
{
	return _sessionIndex;
}

std::string Client::getName() const
{
	return _name;
}

std::string Client::getNickname() const
{
	return _nickname;
}

std::string Client::getUsername() const
{
	return _username;
}

std::string Client::getHostname() const
{
	return _hostname;
}
std::string Client::getServername() const
{
	return _servername;
}

std::string Client::getChannel(std::string channelName)
{
	std::set<std::string>::iterator it = _channels.find(channelName);
	if (it == _channels.end())
	{
		return "";
	}

	return *it;
}

const std::set<std::string> &Client::getChannels() const
{
	return _channels;
}

bool Client::getIsPass() const
{
	return _isPass;
}

bool Client::getIsAdmin() const
{
	return _isAdmin;
}

void Client::setAdminTrue()
{
	_isAdmin = true;
}

void Client::setPassTrue()
{
	_isPass = true;
}

void Client::setName(std::string name)
{
	_name = name;
}

void Client::setNickname(std::string nickname)
{
	_nickname = nickname;
}

void Client::setUsername(std::string username)
{
	_username = username;
}

void Client::setHostname(std::string hostname)
{
	_hostname = hostname;
}

void Client::setServername(std::string servername)
{
	_servername = servername;
}
