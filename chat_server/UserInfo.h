#pragma once

#include <string>
#include <cctype>

// ʵ���࣬�洢��Ҫ�������ݿ���û���Ϣ
struct UserInfo
{
	std::string username;
	std::string password;
	std::string email;
	std::int32_t uid;
};