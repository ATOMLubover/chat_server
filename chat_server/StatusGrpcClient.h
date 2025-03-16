#pragma once

#include "global.h"
#include "StatusRpcConnectionPool.h"

#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetChatServerRequest;
using message::GetChatServerResponse;
using message::LoginRequest;
using message::LoginResponse;
using message::StatusService;

class StatusGrpcClient
	: public Singleton<StatusGrpcClient>
{ 
	friend class Singleton<StatusGrpcClient>;

public:
	~StatusGrpcClient();

private:
	StatusGrpcClient();
};