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

bool MySqlManager::CheckEmail( const std::string& name, const std::string& email )
{
    return dao.CheckEmail( name, email );
}

bool MySqlManager::CheckPassword( const std::string& email, const std::string& pwd, UserInfo* info )
{
    return dao.CheckPassword( email, pwd, info );
}

MySqlManager::MySqlManager()
{ 
    std::cout << "MySqlManager constructed" << std::endl;
}
