#pragma once

#include <functional>

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