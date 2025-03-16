#pragma once

#include "Singleton.h"
#include "MySqlDao.h"
#include "UserInfo.h"

class MySqlManager
	: public Singleton<MySqlManager>
{
	friend class Singleton<MySqlManager>;

public:
	MySqlManager( const MySqlManager& ) = delete;
	MySqlManager& operator=( const MySqlManager& ) = delete;
	~MySqlManager();

	int RegisterUser( const std::string& name, const std::string& email, const std::string& pwd );
	bool CheckEmail( const std::string& name, const std::string& email );
	bool CheckPassword( const std::string& name, const std::string& pwd, UserInfo* info );

private:
	MySqlManager();

private:
	MySqlDao dao;
};