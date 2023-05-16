#include "OperatorManager.hpp"

void OperatorManager::init(std::string operatorName, std::string operatorPassword)
{
    _operatorName = operatorName;
    _operatorPassword = operatorPassword;
}

bool OperatorManager::login(std::string operatorName, std::string operatorPassword)
{
    if (operatorName == _operatorName && operatorPassword == _operatorPassword)
    {
        return true;
    }
    return false;
}