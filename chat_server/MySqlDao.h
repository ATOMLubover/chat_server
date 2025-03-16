#pragma once

#include "UserInfo.h"
#include "MySqlConnectionPool.h"

#include <string>
#include <memory>

class MySqlDao
{ 
public:
	MySqlDao();
	~MySqlDao();

	int RegisterUser( const std::string& name, const std::string& email, const std::string& pwd );
	bool CheckEmail( const std::string& name, const std::string& email );
	bool CheckPassword( const std::string& name, const std::string& pwd, UserInfo* info );

private:
	std::unique_ptr<MySqlConnectionPool> connection_pool;
};