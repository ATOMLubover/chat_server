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

LogicSystem::LogicSystem()
{
	// TODO: for test
	RegisterGet( "/get_test",
				 [] ( std::shared_ptr<HttpConnection> connection )
				 {
					 beast::ostream( connection->response.body() )
						 << "recieved /get_test request";
				 } );
}
