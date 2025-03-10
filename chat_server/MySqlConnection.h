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

// RAII 类, 对 MySQL 连接封装
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

	// 重新建立一个新链接代替原链接
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
	std::int64_t sec_last_oper; // 记录上次增删改查的时间，用于保证 MySQL 服务器不会自动断开连接
};