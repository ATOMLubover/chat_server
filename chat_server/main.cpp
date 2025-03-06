#include "Server.h"
#include "global.h"

#include <Windows.h>

void LoadConfig()
{
	std::ifstream ifs_config( "resources/config.json" );
	std::ostringstream oss_config;
	oss_config << ifs_config.rdbuf();
	std::string str_config;
	str_config = oss_config.str();

	JSON json_config = JSON::parse( str_config );

	port_gate_server = json_config[ "port_gate_server" ].get<std::string>();
	port_verify_client = json_config[ "port_verify_client" ].get<std::string>();
}

#include <cassert>
#include "RedisManager.h"
using namespace std;

void TestRedisManager()
{
	assert( RedisManager::GetInstance()->Connect( "127.0.0.1", "6380" ) );
	assert( RedisManager::GetInstance()->Authorize( "248313" ) );
	assert( RedisManager::GetInstance()->Set( "blogwebsite", "llfc.club" ) );
	std::string value = "";
	assert( RedisManager::GetInstance()->Get( "blogwebsite", &value ) );
	assert( RedisManager::GetInstance()->Get( "nonekey", &value ) == false );
	assert( RedisManager::GetInstance()->HashSet( "bloginfo", "blogwebsite", "llfc.club" ) );
	std::string val;
	RedisManager::GetInstance()->HashGet( "bloginfo", "blogwebsite", &val );
	assert( val != "" );
	assert( RedisManager::GetInstance()->IsKeyExisting( "bloginfo" ) );
	assert( RedisManager::GetInstance()->Delete( "bloginfo" ) );
	assert( RedisManager::GetInstance()->Delete( "bloginfo" ) );
	assert( RedisManager::GetInstance()->IsKeyExisting( "bloginfo" ) == false );
	assert( RedisManager::GetInstance()->LeftPush( "lpushkey1", "lpushvalue1" ) );
	assert( RedisManager::GetInstance()->LeftPush( "lpushkey1", "lpushvalue2" ) );
	assert( RedisManager::GetInstance()->LeftPush( "lpushkey1", "lpushvalue3" ) );
	assert( RedisManager::GetInstance()->RightPop( "lpushkey1", &value ) );
	assert( RedisManager::GetInstance()->RightPop( "lpushkey1", &value ) );
	assert( RedisManager::GetInstance()->LeftPop( "lpushkey1", &value ) );
	assert( RedisManager::GetInstance()->LeftPop( "lpushkey2", &value ) == false );
	RedisManager::GetInstance()->Close();
}

int main( int argc, char* argv[] )
{
	SetConsoleOutputCP( CP_UTF8 );

	TestRedisManager();

	LoadConfig();

	try
	{
		net::io_context ioc{ 1 };

		// stop ioc
		boost::asio::signal_set signals( ioc, SIGINT, SIGTERM );
		signals.async_wait( [&ioc] ( const boost::system::error_code& err, int sig_num )
							{
								if ( err )
									return;

								ioc.stop();
							} );

		unsigned short port = static_cast<unsigned short>( 8080 );
		std::make_shared<Server>( ioc, port )->Start();

		ioc.run(); // run ioc after all init
	}
	catch ( std::exception& e )
	{
		//logger.Log( hatsuiki::SyncFileLogger::EnumLevel::Warning,
		//			"exception({}) occured at main", e.what() );
		std::cout << "exception occurred at main" << std::endl;
		return EXIT_FAILURE;
	}

	return 0;
}