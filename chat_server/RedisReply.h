#pragma once

#include <hiredis.h>

// �� redisReply ���װ�� RAII ��
class RedisReply
{
public:
	using RawRedisReply = redisReply;

public:
	RedisReply( RawRedisReply* reply )
		: reply( reply )
	{ }
	RedisReply( RedisReply&& rhs )
	{
		reply = rhs.reply;
		rhs.reply = nullptr;
	}
	~RedisReply()
	{
		if ( reply )
			freeReplyObject( reply );
	}

	RawRedisReply* GetReply()
	{
		return reply;
	}

	explicit operator bool()
	{
		return reply != nullptr;
	}

	RawRedisReply* operator->()
	{
		return reply;
	}

private:
	RawRedisReply* reply;
};