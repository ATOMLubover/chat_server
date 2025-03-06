#pragma once

#include "Singleton.h"
#include "RedisReply.h"
#include "RedisContext.h"
#include "RedisConnectionPool.h"

#include <hiredis.h>

#include <string>
#include <memory>
#include <vector>
#include <initializer_list>

class RedisManager
	: public Singleton<RedisManager>
{
	friend class Singleton<RedisManager>;

	using ByteList = const char*;

public:
	RedisManager( const RedisManager& ) = delete;
	RedisManager& operator=( const RedisManager& ) = delete;
	~RedisManager();

	void Close();

	bool Get( const std::string& input_key, std::string* output_value );
	bool Set( const std::string& key, const std::string& value );

	// bool Authorize( const std::string& password );

	bool LeftPush( const std::string& key, const std::string& value );
	bool LeftPop( const std::string& input_key, std::string* output_value_popped );
	bool RightPush( const std::string& key, const std::string& value );
	bool RightPop( const std::string& input_key, std::string* output_value_popped );

	bool HashSet( const std::string& key1, const std::string& key2, const std::string& value );
	bool HashSet( const std::string& key1, const std::string& key2, ByteList value_data, std::size_t value_len );
	bool HashGet( const std::string& input_key1, const std::string& input_key2, std::string* output_value );

	bool Delete( const std::string& key );

	bool IsKeyExisting( const std::string& key );

private:
	RedisManager();

	// 传送字符串形式的内容
	RedisReply::RawRedisReply* SendCommand( 
		RedisContext::RawRedisContext* context, const std::string& command );
	// 传入的所有参数可以是字节存储的二进制数据
	RedisReply::RawRedisReply* SendCommandArgv( 
		RedisContext::RawRedisContext* context, const std::initializer_list<ByteList>& argv );

private:
	std::unique_ptr<RedisConnectionPool> connection_pool;
};