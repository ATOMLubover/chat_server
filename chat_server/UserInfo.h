#pragma once

#include <string>
#include <cctype>

// 实体类，存储需要存入数据库的用户信息
struct UserInfo
{
	std::string username;
	std::string password;
	std::string email;
	std::int32_t uid;
};