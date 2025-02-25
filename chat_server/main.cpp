﻿#include "Server.h"
#include "global.h"

#include <Windows.h>

int main( int argc, char* argv[] )
{
	SetConsoleOutputCP( CP_UTF8 );

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
		logger.Log( hatsuiki::SyncFileLogger::EnumLevel::Warning,
					"exception({}) occured at main", e.what() );
		return EXIT_FAILURE;
	}

	return 0;
}