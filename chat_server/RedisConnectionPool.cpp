#include "RedisConnectionPool.h"

#include "RedisReply.h"

#include <iostream>

RedisConnectionPool::RedisConnectionPool(
	std::size_t size, const std::string& host, const std::string& port, const std::string& pwd )
	: size( size ), host( host ), port( port )
	, is_stopped( false )
{
	for ( std::size_t i = 0; i < size; i++ )
	{
		RedisContext context( redisConnect( host.c_str(), std::stoi( port ) ) );
		if ( !context || context->err != 0 )
			continue;

		RedisReply reply( (RedisReply::RawRedisReply*)
						  redisCommand( context.GetContext(), "AUTH %s", pwd.c_str() ) );
		if ( reply->type == REDIS_REPLY_ERROR )
		{
			std::cout << "failed to auth on " << port << ":" << port << std::endl;
			return;
		}
		std::cout << "auth seccessfully on " << port << ":" << port << std::endl;

		connections.push( std::move( context ) );
	}
}

RedisConnectionPool::~RedisConnectionPool()
{
	std::lock_guard<std::mutex> guard( mtx_connections );

	ClosePool();
	while ( !connections.empty() )
		connections.pop();
}

RedisContext::RawRedisContext* RedisConnectionPool::TakeConnection()
{
	std::unique_lock<std::mutex> lock( mtx_connections );

	cv.wait( lock,
			 [this] ()
			 {
				 if ( this->is_stopped )
					 return true;

				 return !this->connections.empty();
			 } );

	if ( is_stopped )
		return nullptr;

	auto context = std::move( connections.front() );
	connections.pop();
	return context.GetContext();
}

void RedisConnectionPool::ReturnConnection( RedisContext::RawRedisContext* context )
{
	std::lock_guard<std::mutex> guard( mtx_connections );

	if ( is_stopped )
		return;

	connections.push( std::move( RedisContext( context ) ) );
	cv.notify_one();
}

void RedisConnectionPool::ClosePool()
{
	is_stopped = true;
	cv.notify_all();
}
