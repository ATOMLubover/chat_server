#pragma once

#include <string>

struct MySqlConnectionInfo
{ 
	std::string url; // MySQL 服务器的地址
	std::string user;
	std::string password;
	std::string schema; // 要使用的数据库
};