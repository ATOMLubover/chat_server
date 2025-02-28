#include "VerifyGrpcClient.h"

GetVerifyResponse VerifyGrpcClient::GetVerificationCode( const std::string& email )
{
	ClientContext context;

	GetVerifyResponse reply;
	GetVerifyRequest request;

	request.set_email( email );

	Status status = stub->GetVerifyCode( &context, request, &reply );

	if ( status.ok() )
	{
		return reply;
	}
	else
	{
		reply.set_error( static_cast<int>( EnumErrorCode::Error_RPC ) );
		return reply;
	}
}

VerifyGrpcClient::VerifyGrpcClient()
{
	std::string target = "127.0.0.1:" + port_verify_client;
	// temp create an insucure channel
	std::shared_ptr<Channel> channel
		= grpc::CreateChannel( target, grpc::InsecureChannelCredentials() );
	std::cout << "channel created on: " << target << std::endl;
	stub = VerifyService::NewStub( channel );
}
