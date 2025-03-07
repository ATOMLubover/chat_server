#include "Server.h"
#include "global.h"

#include <Windows.h>

#include <cassert>
#include "RedisManager.h"
using namespace std;

void TestRedisManager()
{
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