#pragma once

#include <functional>

// ���� Go �е� defer���ڶ���������ʱ����� callback �ص�����
class Defer
{
public:
	Defer( std::function<void()> func )
		: callback( func )
	{ }
	~Defer()
	{
		callback();
	}

private:
	std::function<void()> callback;
};