#pragma once

#include <functional>

// 类似 Go 中的 defer，在对象析构的时候调用 callback 回调函数
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