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
		// ��� conn �ǿ�ָ�룬ֱ�ӿ��ǲ������������
		if ( conn == nullptr )
			return -2;
		// ������ǿ�ָ�룬����Զ���������
		Defer defer( [this, &conn] () 
					 { 
						 this->connection_pool->ReturnConnection( 
							 std::move( conn ) ); 
					 } );

		// ���ô洢����
		std::unique_ptr<sql::PreparedStatement> pstmt( 
			conn->connection->prepareStatement( "CALL reg_user(?,?,?,@result)" ) );
		pstmt->setString( 1, name );
		pstmt->setString( 2, email );
		pstmt->setString( 3, pwd );
		pstmt->execute();

		// ����ִ����䣬����޷�ֱ�ӻ�õ� result
		std::unique_ptr<sql::Statement> stmt_res( conn->connection->createStatement() );
		std::unique_ptr<sql::ResultSet> res( stmt_res->executeQuery( "SELECT @result AS result" ) );
		if ( res->next() )
		{
			int result = res->getInt( "result" );
			std::cout << "result is: " << result << std::endl;
			return result;
		}
		return -1; // û�������Ļ�ý��
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