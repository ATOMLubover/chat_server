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
								  logger.Log( Logger::EnumLevel::Error, "http read error" );
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

			WriteResponse();
			return;
		}

		// other parts are delegated to HandleGet()
		response.result( http::status::ok );
		response.set( http::field::server, "GateServer" );
		WriteResponse();
	}
}

void HttpConnection::WriteResponse()
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

std::string HttpConnection::EncodeUrl( const std::string& raw )
{
	std::ostringstream oss_encoded;

	for ( char ch : raw )
	{
		// only directly input alpha, num, and some common chars
		if ( std::isalnum( ch ) 
			 || ch == '-' || ch == '_' || ch == '.' || ch == '~' )
		{
			oss_encoded << ch;
		}
		else
		{
			oss_encoded 
				<< '%' << std::uppercase << std::hex 
				<< static_cast<int>( static_cast<std::uint8_t>( ch ) );
		}
	}

	return oss_encoded.str();
}

std::string HttpConnection::DecodeUrl( const std::string& url )
{
	std::ostringstream oss_decoded;

	for ( std::size_t i = 0; i < url.size(); i ++ )
	{
		char ch{};

		switch ( url[ i ] )
		{
			case '%':
			{
				std::string hex = url.substr( i + 1, 2 );
				ch = static_cast<char>( std::stoul( hex, nullptr, 16 ) );
				
				i++; // skip two chars after '%'

				break;
			}

			case '+':
			{
				ch = ' ';
				break;
			}
		
			default:
			{
				ch = url[ i ];
				break;
			}
		}

		oss_decoded << ch;
	}

	return oss_decoded.str();
}

std::string HttpConnection::ConvertToUtf8( const std::string& str )
{
	return boost::locale::conv::to_utf<char>( str, "UTF_8" );
}

std::string HttpConnection::EncodeUrlUtf8( const std::string& utf8_str )
{
	std::string url_str;
	uri::encode( utf8_str.begin(), utf8_str.end(), url_str.begin() );
	return url_str;
}

std::string HttpConnection::DecodeUrlUtf8( const std::string& url )
{
	std::string utf8_str;
	uri::decode( url.begin(), url.end(), utf8_str.begin() );
	return utf8_str;
}


