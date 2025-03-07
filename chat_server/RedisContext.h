#pragma once

#include <hiredis.h>

// �� redisContext ���װ�� RAII ��
class RedisContext
{ 
public:
	using RawRedisContext = redisContext;

public:
	RedisContext( RawRedisContext* context)
		: context( context )
	{ }
	RedisContext( RedisContext&& rhs ) 
		= delete;
	~RedisContext()
	{
		if ( context )
			redisFree( context );
	}

	RawRedisContext* GetContext()
	{
		return context;
	}

	RawRedisContext* Release()
	{
		RawRedisContext* con = context;
		context = nullptr;
		return con;
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