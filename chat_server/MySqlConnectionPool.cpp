#include "MySqlConnectionPool.h"

#include "Defer.h"

MySqlConnectionPool::MySqlConnectionPool( const MySqlConnectionInfo& info, int poolsize )
	: login_info( info ), is_stopped( false ), size( poolsize )
{
	for ( std::size_t i = 0; i < poolsize; i++ )
	{
		connections.push( std::move( std::make_unique<MySqlConnection>( info ) ) );
	}

	check_thread = std::thread(
		[this] ()
		{
			while ( !this->is_stopped )
			{
				this->CheckConnections();
				std::this_thread::sleep_for( std::chrono::seconds( 60 ) );
			}
		} );
	check_thread.detach();
}

MySqlConnectionPool::~MySqlConnectionPool()
{
	std::unique_lock<std::mutex> lock( mtx_connections );

	ClosePool();
	while ( !connections.empty() )
		connections.pop();
}

void MySqlConnectionPool::CheckConnections()
{
	std::lock_guard<std::mutex> guard( mtx_connections );

	auto curr_time = std::chrono::steady_clock::now().time_since_epoch();
	std::int64_t sec_curr = std::chrono::duration_cast<std::chrono::seconds>( curr_time ).count();
	std::size_t size_pool = connections.size(); // 不能使用成员函数在 for 循环中获得 size，否则会死循环
	for ( std::size_t i = 0; i < size_pool; i++ )
	{
		std::unique_ptr<MySqlConnection> conn = std::move( connections.front() );
		connections.pop();
		// 在每次循环结束后自动回收（压回）取出的 conn
		Defer defer( [this, &conn]
					 {
						 this->connections.push( std::move( conn ) );
					 } );

		// 如果上次查询时间在最近 5 秒之内，直接跳过
		if ( sec_curr - conn->sec_last_oper < 5 )
			continue;

		try
		{
			std::unique_ptr<sql::Statement> stmt( conn->connection->createStatement() );
			stmt->executeQuery( "SELECT 1" );
			conn->sec_last_oper = sec_curr;

			std::cout << "execute time alive, curr time is" << sec_curr << std::endl;
		}
		catch ( sql::SQLException& exp )
		{
			std::cout << "error at keeping MySQL conn alive: " << exp.what() << std::endl;
			// 创建新链接代替旧链接
			conn->Rebuild();
		}
	}
}

std::unique_ptr<MySqlConnection> MySqlConnectionPool::TakeConnection()
{
	std::unique_lock<std::mutex> lock( mtx_connections );

	cv_connections.wait( lock,
						 [this] ()
						 {
							 if ( this->is_stopped )
								 return true;
							 if ( !this->connections.empty() )
								 return false;
						 } );

	if ( is_stopped )
		return nullptr;

	std::unique_ptr<MySqlConnection> conn( std::move( connections.front() ) );
	connections.pop();
	return conn;
}

void MySqlConnectionPool::ReturnConnection( std::unique_ptr<MySqlConnection>&& connection )
{
	std::unique_lock<std::mutex> lock( mtx_connections );

	if ( is_stopped )
		return;

	connections.push( std::move( connection ) );
	cv_connections.notify_one();
}

void MySqlConnectionPool::ClosePool()
{
	is_stopped = true;
	cv_connections.notify_all();
}
