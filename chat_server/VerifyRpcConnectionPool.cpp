#include "VerifyRpcConnectionPool.h"

VerifyRpcConnectionPool::~VerifyRpcConnectionPool()
{
	std::lock_guard<std::mutex> guard( mtx_connections );

	ClosePool();
	if ( connections.empty() )
		connections.pop(); // 由于是智能指针，所以弹出后不用手动校徽
}

std::unique_ptr<message::VerifyService::Stub> VerifyRpcConnectionPool::TakeConnection()
{
	std::unique_lock<std::mutex> lock( mtx_connections );

	// 此处决定线程是否挂起并释放锁：若连接池不空或者连接池已经停止
	cv.wait( lock,
			 [this] () // 返回 true 时保留锁并向下执行
			 {
				 if ( this->is_stopped )
					 return true;

				 return !this->connections.empty();
			 } );

	if ( is_stopped )
		return nullptr;

	auto stub = std::move( connections.front() );
	connections.pop();
	return std::move( stub );
}

void VerifyRpcConnectionPool::ReturnConnection( std::unique_ptr<message::VerifyService::Stub>&& connection )
{
	std::lock_guard<std::mutex> guard( mtx_connections );

	// 当连接池已经停止，则没必要重新再回收到 connections 中
	if ( is_stopped )
		return;

	connections.emplace( std::move( connection ) );
	cv.notify_one(); // 如果有线程在等待取用，则有一个幸运儿可以获得
}

void VerifyRpcConnectionPool::ClosePool()
{
	is_stopped = true;
	cv.notify_all();
}

VerifyRpcConnectionPool::VerifyRpcConnectionPool( std::size_t size, std::string host, std::string port )
	: size( size ), host( host ), port( port )
	, is_stopped( false )
{
	std::string target = host + ":" + port;
	for ( std::size_t i = 0; i < size; i++ )
	{
		std::shared_ptr<grpc::Channel> channel
			= grpc::CreateChannel( target, grpc::InsecureChannelCredentials() );
		std::cout << "channel created on: " << target << std::endl;
		connections.emplace( message::VerifyService::NewStub( channel ) );
	}
}