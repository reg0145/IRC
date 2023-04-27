#ifndef IRC_Client_HPP
#define IRC_Client_HPP

#include <string>
#include <set>

/* 생성될 때 fd 값도 받아와야하지 않을까?*/
class Client {
	public:
		Client();
    Client(std::string nickname);
		~Client();
		
	private:
		int _fd;
		std::string _clientname;
		std::string _nickname;
		std::string _hostname;
		std::set<std::string> _channels;
		bool isAdmin;
};


#endif