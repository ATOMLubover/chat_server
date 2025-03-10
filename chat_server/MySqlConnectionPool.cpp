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
	std::size_t size_pool = connections.size(); // ����ʹ�ó�Ա������ for ѭ���л�� size���������ѭ��
	for ( std::size_t i = 0; i < size_pool; i++ )
	{
		std::unique_ptr<MySqlConnection> conn = std::move( connections.front() );
		connections.pop();
		// ��ÿ��ѭ���������Զ����գ�ѹ�أ�ȡ���� conn
		Defer defer( [this, &conn]
					 {
						 this->connections.push( std::move( conn ) );
					 } );

		// ����ϴβ�ѯʱ������� 5 ��֮�ڣ�ֱ������
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
			// ���������Ӵ��������
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
