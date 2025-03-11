#pragma once

#include "Singleton.h"
#include "MySqlDao.h"

class MySqlManager
	: public Singleton<MySqlManager>
{
	friend class Singleton<MySqlManager>;

public:
	MySqlManager( const MySqlManager& ) = delete;
	MySqlManager& operator=( const MySqlManager& ) = delete;
	~MySqlManager() = default;

	int RegisterUser( const std::string& name, const std::string& email, const std::string& pwd );

private:
	MySqlManager() = default;

private:
	MySqlDao dao;
};