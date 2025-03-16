#include "MySqlDao.h"

#include "ConfigManager.h"
#include "Defer.h"

MySqlDao::MySqlDao()
{
	MySqlConnectionInfo info_conn =
	{
		ConfigManager::GetInstance()->GetValue( "mysql", "host" ) + ":"
			+ ConfigManager::GetInstance()->GetValue( "mysql", "post" ),
		ConfigManager::GetInstance()->GetValue( "mysql", "user" ),
		ConfigManager::GetInstance()->GetValue( "mysql", "password" ),
		ConfigManager::GetInstance()->GetValue( "mysql", "schema" )
	};
	connection_pool.reset( new MySqlConnectionPool( info_conn, 5 ) );
}

MySqlDao::~MySqlDao()
{
	connection_pool->ClosePool();
}

int MySqlDao::RegisterUser( const std::string& name, const std::string& email, const std::string& pwd )
{
	std::unique_ptr<MySqlConnection> conn = connection_pool->TakeConnection();
	try
	{
		// 如果 conn 是空指针，直接考虑不返还这个连接
		if ( conn == nullptr )
			return -2;
		// 如果不是空指针，最后自动回收连接
		Defer defer( [this, &conn] ()
					 {
						 this->connection_pool->ReturnConnection(
							 std::move( conn ) );
					 } );

		// 调用存储过程
		std::unique_ptr<sql::PreparedStatement> pstmt(
			conn->connection->prepareStatement( "CALL reg_user(?,?,?,@result)" ) );
		pstmt->setString( 1, name );
		pstmt->setString( 2, email );
		pstmt->setString( 3, pwd );
		pstmt->execute();

		// 调用执行语句，获得无法直接获得的 result
		std::unique_ptr<sql::Statement> stmt_res( conn->connection->createStatement() );
		std::unique_ptr<sql::ResultSet> res( stmt_res->executeQuery( "SELECT @result AS result" ) );
		if ( res->next() )
		{
			int result = res->getInt( "result" );
			std::cout << "result is: " << result << std::endl;
			return result;
		}
		return -1; // 没有正常的获得结果
	}
	catch ( sql::SQLException& exp )
	{
		std::cout
			<< "SQLExp: " << exp.what() << std::endl
			<< "( MySQL error code: " << exp.getErrorCode()
			<< ", MySQL state: " << exp.getSQLState() << " )" << std::endl;
		return -1;
	}
}

bool MySqlDao::CheckEmail( const std::string& name, const std::string& email )
{
	auto conn = connection_pool->TakeConnection();
	try 
	{
		// 如果 conn 是空指针，直接考虑不返还这个连接
		if ( conn == nullptr )
			return -2;
		// 如果不是空指针，最后自动回收连接
		Defer defer( [this, &conn] ()
					 {
						 this->connection_pool->ReturnConnection(
							 std::move( conn ) );
					 } );

		// 准备查询语句
		std::unique_ptr<sql::PreparedStatement> pstmt(
			conn->connection->prepareStatement( "SELECT email FROM user WHERE name = ?" ) );
		// 绑定参数
		pstmt->setString( 1, name );
		// 执行查询
		std::unique_ptr<sql::ResultSet> res( pstmt->executeQuery() );

		// 遍历结果集
		while ( res->next() ) {
			std::cout << "Check Email: " << res->getString( "email" ) << std::endl;
			if ( email != res->getString( "email" ) ) {
				return false;
			}
			return true;
		}
		return false;
	}
	catch ( sql::SQLException& exp ) 
	{
		std::cerr << "SQLException: " << exp.what();
		std::cerr << " (MySQL error code: " << exp.getErrorCode();
		std::cerr << ", SQLState: " << exp.getSQLState() << " )" << std::endl;
		return false;
	}
}

bool MySqlDao::CheckPassword( const std::string& email, const std::string& pwd, UserInfo* info )
{
	auto conn = connection_pool->TakeConnection();
	try 
	{
		// 如果 conn 是空指针，直接考虑不返还这个连接
		if ( conn == nullptr )
			return -2;
		// 如果不是空指针，最后自动回收连接
		Defer defer( [this, &conn] ()
					 {
						 this->connection_pool->ReturnConnection(
							 std::move( conn ) );
					 } );

		// 准备查询语句
		std::unique_ptr<sql::PreparedStatement> pstmt(
			conn->connection->prepareStatement( "SELECT * FROM user WHERE email = ?" ) );
		// 绑定参数
		pstmt->setString( 1, email );
		// 执行查询
		std::unique_ptr<sql::ResultSet> res( pstmt->executeQuery() );

		// 遍历结果集
		while ( res->next() ) {
			std::cout << "Check Pwd: " << res->getString( "pwd" ) << std::endl;
			if ( pwd != res->getString( "pwd" ) ) {
				return false;
			}
			break;
		}

		info->username = res->getString( "name" );
		info->email = res->getString( "email" );
		info->uid = res->getInt( "uid" );
		info->password = res->getString( "pwd" ); 
		return true;
	}
	catch ( sql::SQLException& exp ) 
	{
		std::cerr << "SQLException: " << exp.what();
		std::cerr << " (MySQL error code: " << exp.getErrorCode();
		std::cerr << ", SQLState: " << exp.getSQLState() << " )" << std::endl;
		return false;
	}
}
