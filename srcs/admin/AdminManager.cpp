#include "AdminManager.hpp"

void AdminManager::init(std::string adminName, std::string adminPassword)
{
    _adminName = adminName;
    _adminPassword = adminPassword;
}

bool AdminManager::login(std::string adminName, std::string adminPassword)
{
    if (adminName == _adminName && adminPassword == _adminPassword)
    {
        return true;
    }
    return false;
}