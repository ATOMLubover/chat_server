#pragma once

#include <string>

struct MySqlConnectionInfo
{ 
	std::string url; // MySQL �������ĵ�ַ
	std::string user;
	std::string password;
	std::string schema; // Ҫʹ�õ����ݿ�
};