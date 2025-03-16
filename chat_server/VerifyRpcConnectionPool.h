#pragma once

#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"

#include <string>
#include <atomic>
#include <queue>
#include <memory>
#include <condition_variable>

// 作为成员变量使用，而非管理类（内置锁）
class VerifyRpcConnectionPool
{
public:
	VerifyRpcConnectionPool( std::size_t size, std::string host, std::string port );
	VerifyRpcConnectionPool( const VerifyRpcConnectionPool& ) = delete;
	VerifyRpcConnectionPool& operator=( const  VerifyRpcConnectionPool& ) = delete;
	~VerifyRpcConnectionPool();

	std::unique_ptr<message::VerifyService::Stub> TakeConnection();
	void ReturnConnection( std::unique_ptr<message::VerifyService::Stub>&& connection );

private:
	void ClosePool();

private:
	std::atomic<bool> is_stopped;
	std::condition_variable cv;
	std::mutex mtx_connections;
	
	std::string host;
	std::string port;

	std::size_t size;
	std::queue<std::unique_ptr<message::VerifyService::Stub>> connections;
};