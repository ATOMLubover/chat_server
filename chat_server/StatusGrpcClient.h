#pragma once

#include "global.h"
#include "StatusRpcConnectionPool.h"

class StatusGrpcClient
	: public Singleton<StatusGrpcClient>
{ 
	friend class Singleton<StatusGrpcClient>;

public:
	~StatusGrpcClient() = default;

	GetChatServerResponse GetChatServer( int uid );
	// LoginResponse Login( int uid, const std::string& token );

private:
	StatusGrpcClient();

private:
	std::unique_ptr<StatusRpcConnectionPool> connection_pool;
};