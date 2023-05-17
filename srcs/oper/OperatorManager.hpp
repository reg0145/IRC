#ifndef OPERATOR_MANAGER_HPP
#define OPERATOR_MANAGER_HPP

#include <string>

class OperatorManager
{
    public:
        void init(std::string operatorName, std::string operatorPassword);
        bool login(std::string operatorName, std::string operatorPassword);
    
    private:
        std::string _operatorName;
        std::string _operatorPassword;

};

#endif