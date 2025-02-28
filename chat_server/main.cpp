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

int main( int argc, char* argv[] )
{
	SetConsoleOutputCP( CP_UTF8 );

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