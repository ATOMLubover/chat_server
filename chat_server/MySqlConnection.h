#pragma once

#include "MySqlConnectionInfo.h"

#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>

#include <cctype>
#include <memory>
#include <thread>

// RAII ��, �� MySQL ���ӷ�װ
class MySqlConnection
{
public:
	MySqlConnection( const MySqlConnectionInfo& info )
		: info( info )
	{
		try
		{
			sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
			sql::Connection* conn
				= driver->connect( info.url, info.user, info.password );
			conn->setSchema( info.schema );
			connection.reset( conn );

			auto curr_time = std::chrono::steady_clock::now().time_since_epoch();
			sec_last_oper = std::chrono::duration_cast<std::chrono::seconds>( curr_time ).count();
		}
		catch ( std::exception& exp )
		{
			std::cout << "mysql connection construction failed" << std::endl;
		}
	}
	~MySqlConnection()
		= default;

	// ���½���һ�������Ӵ���ԭ����
	void Rebuild()
	{
		try
		{
			sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();
			sql::Connection* conn
				= driver->connect( info.url, info.user, info.password );
			conn->setSchema( info.schema );
			connection.reset( conn );

			auto curr_time = std::chrono::steady_clock::now().time_since_epoch();
			sec_last_oper = std::chrono::duration_cast<std::chrono::seconds>( curr_time ).count();
		}
		catch ( std::exception& exp )
		{
			std::cout << "mysql connection rebuild failed" << std::endl;
		}
	}

public:
	MySqlConnectionInfo info;
	std::unique_ptr<sql::Connection> connection;
	std::int64_t sec_last_oper; // ��¼�ϴ���ɾ�Ĳ��ʱ�䣬���ڱ�֤ MySQL �����������Զ��Ͽ�����
};