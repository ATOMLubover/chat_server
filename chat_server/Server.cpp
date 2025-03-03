#include "Server.h"

#include "AsioIoContextPool.h"
#include "HttpConnection.h"

Server::Server( net::io_context& ioc, unsigned short port )
	: ioc( ioc )
	, acceptor( ioc, tcp::endpoint( tcp::v4(), port ) )
{ }

void Server::Start()
{
	auto self = shared_from_this(); // keep self from being destructed

	auto& io_context = AsioIoContextPool::GetInstance()->GetIoService();
	std::shared_ptr<HttpConnection> connection = std::make_shared<HttpConnection>( io_context );
	acceptor.async_accept( connection->GetSocket(),
						   [self, connection] ( beast::error_code err )
						   {
							   try
							   {
								   // if err is false(no error), 
								   if ( err )
								   {
									   self->Start();
									   return;
								   }

								   connection->Start();

									// start trying to listen to another link
								   self->Start();
							   }
							   catch ( std::exception& e )
							   {
								   std::cout << "exception occurred at Server Start() callback" << std::endl;
							   }
						   } );
}