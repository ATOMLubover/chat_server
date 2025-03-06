#pragma once

#include "RedisContext.h"

#include <hiredis.h>

#include <string>
#include <atomic>
#include <queue>
#include <memory>
#include <condition_variable>

// 作为成员变量使用（内置锁）
class RedisConnectionPool
{
public:
	RedisConnectionPool( std::size_t size, const std::string& host, const std::string& port, const std::string& pwd );
	RedisConnectionPool( const RedisConnectionPool& ) = delete;
	RedisConnectionPool& operator=( const  RedisConnectionPool& ) = delete;
	~RedisConnectionPool();

	RedisContext::RawRedisContext* TakeConnection();
	void ReturnConnection( RedisContext::RawRedisContext* context );

	void ClosePool();

private:
	std::atomic<bool> is_stopped;
	std::condition_variable cv;
	std::mutex mtx_connections;

	std::string host;
	std::string port;

	std::size_t size;
	std::queue<RedisContext> connections;
};