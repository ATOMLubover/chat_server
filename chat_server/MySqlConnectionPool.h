#pragma once

#include "MySqlConnection.h"
#include "MySqlConnectionInfo.h"

#include <string>
#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>

class MySqlConnectionPool
{
public:
	MySqlConnectionPool( const MySqlConnectionInfo& info, int poolsize );
	~MySqlConnectionPool();

	// ��֤���Ӳ��ᳬʱ�Ͽ�
	void CheckConnections();

	std::unique_ptr<MySqlConnection> TakeConnection();
	void ReturnConnection( std::unique_ptr<MySqlConnection>&& connection );

	void ClosePool();

private:
	MySqlConnectionInfo login_info;

	std::atomic<bool> is_stopped;

	int size;
	std::queue<std::unique_ptr<MySqlConnection>> connections;
	std::mutex mtx_connections;
	std::condition_variable cv_connections;

	std::thread check_thread; // ��֤���ݿ����Ӳ����Զ��жϵļ���߳�
};