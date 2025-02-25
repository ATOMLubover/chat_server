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
	auto iter_handler = post_handlers.find( url );

	if ( iter_handler == post_handlers.end() )
		return false;

	iter_handler->second( connection );
	return true;
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

	RegisterPost( "/get_verification_code",
				  [] ( std::shared_ptr<HttpConnection> connection )
				  {
					  auto str_body = beast::buffers_to_string( connection->request.body().data() );
					  std::cout << "recv body is: " << str_body << std::endl;

					  // recieves a json, returns a json string
					  connection->response.set( http::field::content_type, "text/json" );
					  JSON json_res;

					  JSON json_body = JSON::parse( str_body );

					  if ( json_body.is_null() )
					  {
						  std::cout << "json parse failed" << std::endl;

						  json_res[ "Error" ] = ErrorCode::Error_Json;
						  beast::ostream( connection->response.body() ) << json_res.dump();

						  return true; // not enough to throw false
					  }

					  if ( !json_body.contains( "email" ) )
					  {
						  std::cout << "json 'email' not found" << std::endl;

						  json_res[ "Error" ] = ErrorCode::Error_Json;
						  beast::ostream( connection->response.body() ) << json_res.dump();

						  return true; // not enough to throw false
					  }

					  auto email = json_body[ "email" ].get<std::string>();
					  json_res[ "error" ] = ErrorCode::Success;

					  beast::ostream( connection->response.body() ) << json_res.dump();

					  return true;
				  } );
}
