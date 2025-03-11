#include "MySqlManager.h"

int MySqlManager::RegisterUser( const std::string& name, const std::string& email, const std::string& pwd )
{
    return dao.RegisterUser( name, email, pwd );
}
