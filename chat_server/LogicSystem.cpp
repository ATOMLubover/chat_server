#include "LogicSystem.h"

#include "HttpConnection.h"

void LogicSystem::RegisterGet( const std::string& name_handler, HttpHandler handler )
{
	get_handlers.insert( std::make_pair( name_handler, handler ) );
}

bool LogicSystem::HandleGet( const std::string& url, std::shared_ptr<HttpConnection> connection )
{
	auto iter_handler = get_handlers.find( url );

	if ( iter_handler == get_handlers.end() )
		return false;

	iter_handler->second( connection );
	return true;
}

void LogicSystem::RegisterPost( const std::string& name_handler, HttpHandler handler )
{ 
	post_handlers.insert( std::make_pair( name_handler, handler ) );
}

bool LogicSystem::HandlePost( const std::string& url, std::shared_ptr<HttpConnection> connection )
{
	return false;
}

LogicSystem::LogicSystem()
{
	// TODO: for test
	RegisterGet( "/get_test",
				 [] ( std::shared_ptr<HttpConnection> connection )
				 {
					 connection->response.set( http::field::content_type, "text/plain; charset=utf-8" );

					 beast::ostream( connection->response.body() )
						 << "recieved /get_test request"
						 << std::endl;

					 int i = 0;
					 for ( auto& elem : connection->get_params )
					 {
						 i++;
						 beast::ostream( connection->response.body() )
							 << std::format( "param {}: key=\"{}\", val=\"{}\"",
											 i, elem.first, elem.second )
							 << std::endl;
					 }
				 } );

	RegisterPost( "/post_test",
				  [] ( std::shared_ptr<HttpConnection> connection )
				  {
					  auto body_str = beast::buffers_to_string( connection->request.body().data() );
					  std::cout << "recv body is: " << body_str << std::endl;

					  connection->response.set( http::field::content_type, "text/json" );

					  JSON json_body = JSON::parse( body_str );
					  bool is_success = !json_body.is_null();

					  if ( !is_success )
					  {
						  std::cout << "json parse failed" << std::endl;

						  JSON json_res;
						  json_res[ "Error" ] = ErrorCode::Error_Json;
						  std::string str_res = json_res.dump( 4 );
						  beast::ostream( connection->response.body() ) << str_res;

						  return true; // not enough to throw false
					  }
				  } );
}
