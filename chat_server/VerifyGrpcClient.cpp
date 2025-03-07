#include "VerifyGrpcClient.h"

#include "ConfigManager.h"

GetVerifyResponse VerifyGrpcClient::GetVerificationCode( const std::string& email )
{
	ClientContext context;

	GetVerifyResponse reply;
	GetVerifyRequest request;

	request.set_email( email );

	auto stub = connection_pool.TakeConnection();
	Status status = stub->GetVerifyCode( &context, request, &reply );

	if ( !status.ok() )
	{
		reply.set_error( static_cast<int>( EnumErrorCode::Error_RPC ) );
	}

	connection_pool.ReturnConnection( std::move( stub ) );
	return reply;
}

VerifyGrpcClient::VerifyGrpcClient()
	: connection_pool( 3, "127.0.0.1", ConfigManager::GetInstance()->GetValue( "verify_server", "port" ) )
{ }
