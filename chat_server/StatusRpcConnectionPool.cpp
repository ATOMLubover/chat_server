#include "StatusRpcConnectionPool.h"

StatusRpcConnectionPool::StatusRpcConnectionPool( std::size_t size, std::string host, std::string port )
	: size( size ), host( host ), port( port )
	, is_stopped( false )
{
	std::string target = host + ":" + port;
	for ( std::size_t i = 0; i < size; i++ )
	{
		std::shared_ptr<grpc::Channel> channel
			= grpc::CreateChannel( target, grpc::InsecureChannelCredentials() );
		std::cout << "channel created on: " << target << std::endl;
		connections.emplace( message::StatusService::NewStub( channel ) );
	}
}
StatusRpcConnectionPool::~StatusRpcConnectionPool()
{
	std::lock_guard<std::mutex> guard( mtx_connections );

	ClosePool();
	if ( connections.empty() )
		connections.pop(); // ����������ָ�룬���Ե��������ֶ�У��
}

std::unique_ptr<message::StatusService::Stub> StatusRpcConnectionPool::TakeConnection()
{
	std::unique_lock<std::mutex> lock( mtx_connections );

	// �˴������߳��Ƿ�����ͷ����������ӳز��ջ������ӳ��Ѿ�ֹͣ
	cv.wait( lock,
			 [this] () // ���� true ʱ������������ִ��
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

void StatusRpcConnectionPool::ReturnConnection( std::unique_ptr<message::StatusService::Stub>&& connection )
{
	std::lock_guard<std::mutex> guard( mtx_connections );

	// �����ӳ��Ѿ�ֹͣ����û��Ҫ�����ٻ��յ� connections ��
	if ( is_stopped )
		return;

	connections.emplace( std::move( connection ) );
	cv.notify_one(); // ������߳��ڵȴ�ȡ�ã�����һ�����˶����Ի��
}

void StatusRpcConnectionPool::ClosePool()
{
	is_stopped = true;
	cv.notify_all();
}
