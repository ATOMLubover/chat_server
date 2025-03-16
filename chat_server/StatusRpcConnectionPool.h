#pragma once

#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"

#include <queue>

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetChatServerRequest;
using message::GetChatServerResponse;
using message::LoginRequest;
using message::LoginResponse;
using message::StatusService;

// 作为成员变量使用（内置锁）
class StatusRpcConnectionPool
{ 
public:
	StatusRpcConnectionPool( std::size_t size, std::string host, std::string port );
	StatusRpcConnectionPool( const StatusRpcConnectionPool& ) = delete;
	StatusRpcConnectionPool& operator=( const  StatusRpcConnectionPool& ) = delete;
	~StatusRpcConnectionPool();

	std::unique_ptr<message::StatusService::Stub> TakeConnection();
	void ReturnConnection( std::unique_ptr<message::StatusService::Stub>&& connection );

private:
	void ClosePool();

private:
	std::atomic<bool> is_stopped;
	std::condition_variable cv;
	std::mutex mtx_connections;

	std::string host;
	std::string port;

	std::size_t size;
	std::queue<std::unique_ptr<message::StatusService::Stub>> connections;
};