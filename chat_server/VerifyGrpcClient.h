#pragma once

#include "global.h"
#include "VerifyRpcConnectionPool.h"

#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetVerifyRequest;
using message::GetVerifyResponse;
using message::VerifyService;

class VerifyGrpcClient : public Singleton<VerifyGrpcClient>
{ 
	friend class Singleton<VerifyGrpcClient>;

public:
	GetVerifyResponse GetVerificationCode( const std::string& email );

private:
	VerifyGrpcClient();

private:
	VerifyRpcConnectionPool connection_pool;
};