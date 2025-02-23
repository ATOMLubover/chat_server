#include "HttpConnection.h"

#include "LogicSystem.h"

HttpConnection::HttpConnection( tcp::socket&& socket )
	: socket( std::move( socket ) )
{ }

void HttpConnection::Start()
{ 
	auto self = shared_from_this();

	http::async_read( socket, buf_recv, request,
					  [self] ( beast::error_code err, std::size_t size_bytes )
					  {
						  try
						  {
							  if ( err )
							  {
								  logger.Log( Logger::EnumLevel::Error, "http read error({})", err.what() );
								  return;
							  }

							  // ignore size coz it is http connection
							  boost::ignore_unused( size_bytes );

							  self->HandleRequest();
							  self->CheckTimeout();

							  logger.Log( Logger::EnumLevel::Info, "http read successful" );
						  }
						  catch ( std::exception& e )
						  {
							  logger.Log( Logger::EnumLevel::Warning, 
										  "exception({}) occured at HttpConnection Start() callback",
										  e.what() );
						  }
					  } );
}

void HttpConnection::CheckTimeout()
{ 
	auto self = shared_from_this();

	timer_timeout.async_wait( [self] ( beast::error_code err )
							  {
								  if ( !err )
								  {
									  self->socket.close();

									  logger.Log( Logger::EnumLevel::Error, "timeout at connetion" );
								  }
							  } );
}

void HttpConnection::HandleRequest()
{ 
	response.version( request.version() );
	response.keep_alive( false ); // short connection

	if ( request.method() == http::verb::get )
	{
		bool is_successful 
			= LogicSystem::GetInstance()->HandleGet( request.target(), shared_from_this() );
		if ( !is_successful )
		{
			response.result( http::status::not_found );
			response.set( http::field::content_type, "text/plain" );
			beast::ostream( response.body() ) << "url not found\r\n";

			Respond();
			return;
		}

		// other parts are delegated to HandleGet()
		response.result( http::status::ok );
		response.set( http::field::server, "GateServer" );
		Respond();
	}
}

void HttpConnection::Respond()
{ 
	auto self = shared_from_this();

	response.content_length( response.body().size() );

	http::async_write( socket, response,
					   [self] ( beast::error_code err, std::size_t size_bytes )
					   {
						   // whether the connection is successful, shutdown
						   self->socket.shutdown( tcp::socket::shutdown_send, err ); // shutdown send end only
						   self->timer_timeout.cancel();
					   } );
}
