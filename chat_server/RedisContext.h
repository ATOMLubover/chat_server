#pragma once

#include <hiredis.h>

// 对 redisContext 类封装的 RAII 类
class RedisContext
{ 
public:
	using RawRedisContext = redisContext;

public:
	RedisContext( RawRedisContext* context)
		: context( context)
	{ }
	RedisContext( RedisContext&& rhs ) noexcept
	{
		context = rhs.context;
		rhs.context = nullptr;
	}
	~RedisContext()
	{
		if ( context )
			redisFree( context );
	}

	RawRedisContext* GetContext()
	{
		return context;
	}

	explicit operator bool()
	{
		return context != nullptr;
	}

	RawRedisContext* operator->()
	{
		return context;
	}

private:
	RawRedisContext* context;
};