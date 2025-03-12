#include "MySqlManager.h"

#include <iostream>

MySqlManager::~MySqlManager()
{ 
    std::cout << "MySqlManager deconstructed" << std::endl;
}

int MySqlManager::RegisterUser( const std::string& name, const std::string& email, const std::string& pwd )
{
    return dao.RegisterUser( name, email, pwd );
}

MySqlManager::MySqlManager()
{ 
    std::cout << "MySqlManager constructed" << std::endl;
}
