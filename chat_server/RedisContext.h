#pragma once

#include <hiredis.h>

// �� redisContext ���װ�� RAII ��
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