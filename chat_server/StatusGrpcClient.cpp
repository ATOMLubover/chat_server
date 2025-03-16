#include "StatusGrpcClient.h"

#include "Defer.h"
#include "ConfigManager.h"

GetChatServerResponse StatusGrpcClient::GetChatServer( int uid )
{
	ClientContext context;
	GetChatServerResponse reply;
	GetChatServerRequest request;
	request.set_uid( uid );
	auto stub = connection_pool->TakeConnection();
	Status status = stub->GetChatServer( &context, request, &reply );
	Defer defer( [&stub, this] ()
				 {
					 this->connection_pool->ReturnConnection( std::move( stub ) );
				 } );
	if ( status.ok() ) {
		return reply;
	}
	else {
		reply.set_error( (int) EnumErrorCode::ErrorRpc );
		return reply;
	}
}

StatusGrpcClient::StatusGrpcClient()
{
	std::string host = ConfigManager::GetInstance()->GetValue( "status_server", "host" );
	std::string port = ConfigManager::GetInstance()->GetValue( "status_server", "port" );
	connection_pool.reset( new StatusRpcConnectionPool( 5, host, port ) );
}
