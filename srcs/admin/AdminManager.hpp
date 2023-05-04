#ifndef ADMIN_MANAGER_HPP
#define ADMIN_MANAGER_HPP

#include <string>

class AdminManager
{
    public:
        void init(std::string adminName, std::string adminPassword);
        bool login(std::string adminName, std::string adminPassword);
    
    private:
        std::string _adminName;
        std::string _adminPassword;

};

#endif