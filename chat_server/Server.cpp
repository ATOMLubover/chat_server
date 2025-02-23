#include "Server.h"

#include "HttpConnection.h"

Server::Server( net::io_context& ioc, unsigned short port )
	: ioc( ioc )
	, acceptor( ioc, tcp::endpoint( tcp::v4(), port ) )
	, socket( ioc )
{ }

void Server::Start()
{
	auto self = shared_from_this(); // keep self from being destructed

	acceptor.async_accept( socket,
						   [self] ( beast::error_code err )
						   {
							   try
							   {
								   // if err is false(no error), 
								   if ( !err )
								   {
									   self->Start();
									   // create a link and let HttpConnection to manage it
									   std::make_shared<HttpConnection>( std::move( self->socket ) )->Start();
								   }

									// start trying to listen to another link
								   self->Start();
							   }
							   catch ( std::exception& e )
							   {
								   logger.Log( Logger::EnumLevel::Warning, "exception occured at Server Start()" );
							   }
						   } );
}