#include "RedisManager.h"

#include "ConfigManager.h"

#include <format>

RedisManager::~RedisManager()
{
	Close();
}

//bool RedisManager::Connect( const std::string& host, const std::string& post )
//{
//	connection = redisConnect( host.c_str(), std::stoi( post ) );
//	// �� connection Ϊ�գ������� err ��Ϊ 0 �򷵻ش��� false
//	if ( connection == nullptr
//		 || ( connection != nullptr && connection->err ) )
//	{
//		std::cout << "redis connect error: "
//			<< ( connection ? connection->errstr : "connection is nullptr" ) << std::endl;
//		return false;
//	}
//	return true;
//}

void RedisManager::Close()
{
	connection_pool->ClosePool();
}

bool RedisManager::Get( const std::string& input_key, std::string* output_value )
{
	RedisContext::RawRedisContext* connection = connection_pool->TakeConnection();
	if ( !connection )
		return false;

	std::string command = std::format( "GET {}", input_key );
	std::string log_failure = std::format( "[ {} ] failed ", command );

	RedisReply reply( SendCommand( connection, command ) );
	if ( !reply )
	{
		std::cout << log_failure << std::endl;
		return false;
	}
	if ( reply->type != REDIS_REPLY_STRING )
	{
		std::cout << log_failure << std::endl;
		return false;
	}
	std::string log_success = std::format( "[ {} ] executed successfully", command );
	std::cout << log_success << std::endl;

	connection_pool->ReturnConnection( connection );

	if ( output_value )
		*output_value = reply->str; // ���� key ��Ӧ�� value

	return true;
}

bool RedisManager::Set( const std::string& key, const std::string& value )
{
	RedisContext::RawRedisContext* connection = connection_pool->TakeConnection();
	if ( !connection )
		return false;

	std::string command = std::format( "SET {} {}", key, value );
	std::string log_failure = std::format( "[ {} ] failed ", command );

	RedisReply reply( SendCommand( connection, command ) );
	if ( !reply )
	{
		std::cout << log_failure << std::endl;
		return false;
	}
	bool is_ok
		= reply->type == REDIS_REPLY_STATUS
		&& ( strcmp( reply->str, "OK" ) == 0 /* || strcmp( reply->str, "ok" ) == 0 */ );
	if ( !is_ok )
	{
		std::cout << log_failure << std::endl;
		return false;
	}
	std::string log_success = std::format( "[ {} ] executed successfully", command );
	std::cout << log_success << std::endl;

	connection_pool->ReturnConnection( connection );

	return true;
}

//bool RedisManager::Authorize( const std::string& password )
//{
//	std::string command = std::format( "AUTH {}", password );
//	RedisReply reply( SendCommand( command ) );
//	if ( !reply )
//	{
//		std::cout << "authorize failed" << std::endl;
//		return false;
//	}
//	if ( reply->type == REDIS_REPLY_ERROR )
//	{
//		std::cout << "authorize failed" << std::endl;
//		return false;
//	}
//
//	std::cout << "authorize succeeded" << std::endl;
//	return true;
//}

bool RedisManager::LeftPush( const std::string& key, const std::string& value )
{
	RedisContext::RawRedisContext* connection = connection_pool->TakeConnection();
	if ( !connection )
		return false;

	std::string command = std::format( "LPUSH {} {}", key, value );
	std::string log_failure = std::format( "[ {} ] failed", command );

	RedisReply reply( SendCommand( connection,  command ) );
	if ( !reply )
	{
		std::cout << log_failure << std::endl;
		return false;
	}
	// �� LPUSH �ɹ�ʱ������ֵ�ǲ���/���µĶ��еĳ���
	if ( reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0 )
	{
		std::cout << log_failure << std::endl;
		return false;
	}
	std::string log_success = std::format( "[ {} ] executed successfully", command );
	std::cout << log_success << std::endl;

	connection_pool->ReturnConnection( connection );

	return true;
}

bool RedisManager::LeftPop( const std::string& input_key, std::string* output_value_popped )
{
	RedisContext::RawRedisContext* connection = connection_pool->TakeConnection();
	if ( !connection )
		return false;

	std::string command = std::format( "LPOP {}", input_key );
	std::string log_failure = std::format( "[ {} ] failed", command );

	RedisReply reply( SendCommand( connection, command ) );
	if ( !reply )
	{
		std::cout << log_failure << std::endl;
		return false;
	}
	if ( reply->type == REDIS_REPLY_NIL )
	{
		std::cout << log_failure << std::endl;
		return false;
	}
	std::string log_success = std::format( "[ {} ] executed successfully", command );
	std::cout << log_success << std::endl;

	connection_pool->ReturnConnection( connection );

	if ( output_value_popped )
		*output_value_popped = reply->str;

	return true;
}

bool RedisManager::RightPush( const std::string& key, const std::string& value )
{
	RedisContext::RawRedisContext* connection = connection_pool->TakeConnection();
	if ( !connection )
		return false;

	std::string command = std::format( "RPUSH {} {}", key, value );
	std::string log_failure = std::format( "[ {} ] failed", command );

	RedisReply reply( SendCommand( connection, command ) );
	if ( !reply )
	{
		std::cout << log_failure << std::endl;
		return false;
	}
	// �� RPUSH �ɹ�ʱ������ֵͬ���ǲ���/���µĶ��еĳ���
	if ( reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0 )
	{
		std::cout << log_failure << std::endl;
		return false;
	}
	std::string log_success = std::format( "[ {} ] executed successfully", command );
	std::cout << log_success << std::endl;

	connection_pool->ReturnConnection( connection );

	return true;
}

bool RedisManager::RightPop( const std::string& input_key, std::string* output_value_popped )
{
	RedisContext::RawRedisContext* connection = connection_pool->TakeConnection();
	if ( !connection )
		return false;

	std::string command = std::format( "LPOP {}", input_key );
	std::string log_failure = std::format( "[ {} ] failed", command );

	RedisReply reply( SendCommand( connection, command ) );
	if ( !reply )
	{
		std::cout << log_failure << std::endl;
		return false;
	}
	if ( reply->type == REDIS_REPLY_NIL )
	{
		std::cout << log_failure << std::endl;
		return false;
	}
	std::string log_success = std::format( "[ {} ] executed successfully", command );
	std::cout << log_success << std::endl;

	connection_pool->ReturnConnection( connection );

	if ( output_value_popped )
		*output_value_popped = reply->str;

	return true;
}

bool RedisManager::HashSet( const std::string& key1, const std::string& key2, const std::string& value )
{
	RedisContext::RawRedisContext* connection = connection_pool->TakeConnection();
	if ( !connection )
		return false;

	std::string command = std::format( "HSET {} {} {}", key1, key2, value );
	std::string log_failure = std::format( "[ {} ] failed", command );

	RedisReply reply( SendCommand( connection, command ) );
	if ( !reply )
	{
		std::cout << log_failure << std::endl;
		return false;
	}
	if ( reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0 )
	{
		std::cout << log_failure << std::endl;
		return false;
	}
	std::string log_success = std::format( "[ {} ] executed successfully", command );
	std::cout << log_success << std::endl;

	connection_pool->ReturnConnection( connection );

	return true;
}

bool RedisManager::HashSet(
	const std::string& key1, const std::string& key2,
	ByteList value_data, std::size_t value_len )
{
	RedisContext::RawRedisContext* connection = connection_pool->TakeConnection();
	if ( !connection )
		return false;

	std::string str_data( value_data, value_len );
	std::string command_log = std::format( "HSET {} {} <{} bytes data>", key1, key2, value_len );
	std::string log_failure = std::format( "[ {} ] failed", command_log );

	RedisReply reply( SendCommandArgv( connection,  { "HSET", key1.c_str(), key2.c_str(), str_data.c_str() } ) );
	if ( !reply )
	{
		std::cout << log_failure << std::endl;
		return false;
	}
	if ( reply->type != REDIS_REPLY_INTEGER )
	{
		std::cout << log_failure << std::endl;
		return false;
	}
	std::string log_success = std::format( "[ {} ] executed successfully", command_log );
	std::cout << log_success << std::endl;

	connection_pool->ReturnConnection( connection );

	return true;
}

bool RedisManager::HashGet( const std::string& input_key1, const std::string& input_key2, std::string* output_value )
{
	RedisContext::RawRedisContext* connection = connection_pool->TakeConnection();
	if ( !connection )
		return false;

	std::string command = std::format( "HGET {} {}", input_key1, input_key2 );
	std::string log_failure = std::format( "[ {} ] failed", command );

	RedisReply reply( SendCommand( connection, command ) );
	if ( !reply )
	{
		std::cout << log_failure << std::endl;
		return false;
	}
	if ( reply->type == REDIS_REPLY_NIL )
	{
		std::cout << log_failure << std::endl;
		return false;
	}
	std::string log_success = std::format( "[ {} ] executed successfully", command );
	std::cout << log_success << std::endl;

	connection_pool->ReturnConnection( connection );

	if ( output_value )
		*output_value = reply->str;

	return true;
}

bool RedisManager::Delete( const std::string& key )
{
	RedisContext::RawRedisContext* connection = connection_pool->TakeConnection();
	if ( !connection )
		return false;

	std::string command = std::format( "DEL {}", key );
	std::string log_failure = std::format( "[ {} ] failed", command );

	RedisReply reply( SendCommand( connection, command ) );
	if ( !reply )
	{
		std::cout << log_failure << std::endl;
		return false;
	}
	if ( reply->type != REDIS_REPLY_INTEGER )
	{
		std::cout << log_failure << std::endl;
		return false;
	}
	std::string log_success = std::format( "[ {} ] executed successfully", command );
	std::cout << log_success << std::endl;

	connection_pool->ReturnConnection( connection );

	return true;
}

bool RedisManager::IsKeyExisting( const std::string& key )
{
	RedisContext::RawRedisContext* connection = connection_pool->TakeConnection();
	if ( !connection )
		return false;

	std::string command = std::format( "exists {}", key );

	RedisReply reply( SendCommand( connection, command ) );
	if ( !reply )
	{
		std::cout << std::format( "Not found [ Key = {} ]", key ) << std::endl;
		return false;
	}
	if ( reply->type != REDIS_REPLY_INTEGER || reply->integer == 0 )
	{
		std::cout << std::format( "Not found [ Key = {} ]", key ) << std::endl;
		return false;
	}
	std::cout << std::format( "Found [ Key = {} ]", command ) << std::endl;

	connection_pool->ReturnConnection( connection );

	return true;
}

RedisManager::RedisManager()
{
	std::string host = ConfigManager::GetInstance()->GetValue( "redis", "host" );
	std::string port = ConfigManager::GetInstance()->GetValue( "redis", "port" );
	std::string pwd = ConfigManager::GetInstance()->GetValue( "redis", "password" );
	connection_pool = std::make_unique< RedisConnectionPool>( 5, host, port, pwd );
}

// �����ַ�����ʽ������
inline RedisReply::RawRedisReply* RedisManager::SendCommand( 
	RedisContext::RawRedisContext* connection, const std::string & command )
{
	return (RedisReply::RawRedisReply*) redisCommand( connection, command.c_str() );
}

// ��������в����������ֽڴ洢�Ķ���������
RedisReply::RawRedisReply* RedisManager::SendCommandArgv( 
	RedisContext::RawRedisContext* connection, const std::initializer_list<ByteList>& argv )
{

	std::vector<std::size_t> vec_argvlen;
	for ( const auto& arg : argv )
		vec_argvlen.push_back( strlen( arg ) );
	std::vector<ByteList> vec_argv( argv );

	ByteList* array_argv = vec_argv.data();
	const std::size_t* array_argvlen = vec_argvlen.data();

	return (RedisReply::RawRedisReply*) redisCommandArgv(
		connection,
		(int) vec_argv.size(), array_argv, array_argvlen );
}
