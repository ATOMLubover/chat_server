#include "LogicSystem.h"

#include "HttpConnection.h"
#include "VerifyGrpcClient.h"
#include "RedisManager.h"
#include "ConfigManager.h"
#include "MySqlManager.h"

void LogicSystem::RegisterGet( const std::string& name_handler, HttpHandler handler )
{
	get_handlers.insert( std::make_pair( name_handler, handler ) );
}

bool LogicSystem::HandleGet( const std::string& url, std::shared_ptr<HttpConnection> connection )
{
	auto iter_handler = get_handlers.find( url );

	if ( iter_handler == get_handlers.end() )
		return false;

	iter_handler->second( connection );
	return true;
}

void LogicSystem::RegisterPost( const std::string& name_handler, HttpHandler handler )
{
	post_handlers.insert( std::make_pair( name_handler, handler ) );
}

bool LogicSystem::HandlePost( const std::string& url, std::shared_ptr<HttpConnection> connection )
{
	auto iter_handler = post_handlers.find( url );

	if ( iter_handler == post_handlers.end() )
		return false;

	iter_handler->second( connection );
	return true;
}

LogicSystem::LogicSystem()
{
	// TODO: for test
	RegisterGet( "/get_test",
				 [] ( std::shared_ptr<HttpConnection> connection )
				 {
					 connection->response.set( http::field::content_type, "text/plain; charset=utf-8" );

					 beast::ostream( connection->response.body() )
						 << "recieved /get_test request"
						 << std::endl;

					 int i = 0;
					 for ( auto& elem : connection->get_params )
					 {
						 i++;
						 beast::ostream( connection->response.body() )
							 << std::format( "param {}: key=\"{}\", val=\"{}\"",
											 i, elem.first, elem.second )
							 << std::endl;
					 }
				 } );

	RegisterPost( "/get_verification_code",
				  [] ( std::shared_ptr<HttpConnection> connection )
				  {
					  auto str_body = beast::buffers_to_string( connection->request.body().data() );
					  std::cout << "recv body is: " << str_body << std::endl;

					  // recieves a json, returns a json string
					  connection->response.set( http::field::content_type, "text/json" );
					  JSON json_res;

					  JSON json_body = JSON::parse( str_body );

					  if ( json_body.is_null() )
					  {
						  std::cout << "json parse failed" << std::endl;

						  json_res[ "Error" ] = EnumErrorCode::ErrorJson;
						  json_res.emplace( JSON{ "email", json_body[ "email" ].get<std::string>() } );
						  beast::ostream( connection->response.body() ) << json_res.dump();

						  return true; // not enough to throw false
					  }

					  if ( !json_body.contains( "email" ) )
					  {
						  std::cout << "json 'email' not found" << std::endl;

						  json_res[ "Error" ] = EnumErrorCode::ErrorJson;
						  json_res.emplace( JSON{ "email", json_body[ "email" ].get<std::string>() } );
						  beast::ostream( connection->response.body() ) << json_res.dump();

						  return true; // not enough to throw false
					  }

					  auto email = json_body[ "email" ].get<std::string>();
					  // make VerificationClient to send vericode
					  GetVerifyResponse veri_response
						  = VerifyGrpcClient::GetInstance()->GetVerificationCode( email );

					  json_res[ "error" ] = veri_response.error();
					  json_res.emplace( JSON{ "email", json_body[ "email" ].get<std::string>() } );

					  beast::ostream( connection->response.body() ) << json_res.dump();

					  return true;
				  } );

	RegisterPost( "/user_register",
				  [] ( std::shared_ptr<HttpConnection> connection )
				  {
					  std::string str_body = boost::beast::buffers_to_string( connection->request.body().data() );
					  std::cout << "receive body is " << str_body << std::endl;

					  connection->response.set( http::field::content_type, "text/json" );

					  JSON json_body = JSON::parse( str_body );
					  if ( json_body.is_null() ) {
						  std::cout << "Failed to parse JSON data!" << std::endl;
						  JSON json_res;
						  json_res.emplace( "error", EnumErrorCode::ErrorJson );
						  beast::ostream( connection->response.body() ) << json_res.dump();

						  return true;
					  }

					  std::string name = json_body[ "user" ].get<std::string>();
					  std::string email = json_body[ "email" ].get<std::string>();
					  std::string pwd = json_body[ "password" ].get<std::string>();
					  std::string verify_code_cli = json_body[ "verify_code" ].get<std::string>();

					  JSON json_res;

					  // 先查找 redis 中 email 对应的验证码是否合理
					  std::string code_prefix
						  = ConfigManager::GetInstance()->GetValue( "const", "code_prefix" );
					  std::string verify_code;
					  bool is_verification_gotton
						  = RedisManager::GetInstance()->Get(
							  code_prefix + email,
							  &verify_code );
					  if ( !is_verification_gotton ) {
						  std::cout << "get varify code expired" << std::endl;
						  json_res.emplace( "error", EnumErrorCode::ErrorRedisExpired );
						  beast::ostream( connection->response.body() ) << json_res.dump();

						  return true;
					  }

					  // 如果 redis 中的验证码与客户端传过来的不相同
					  if ( verify_code != verify_code_cli ) {
						  std::cout << " verify code error" << std::endl;
						  json_res.emplace( "error", (int) EnumErrorCode::ErrorVerify );
						  beast::ostream( connection->response.body() ) << json_res.dump();

						  return true;
					  }

					  // 在 mysql 中查找用户是否存在
					  int uid = MySqlManager::GetInstance()->RegisterUser( name, email, pwd );
					  if ( uid == 0 || uid == -1 )
					  {
						  std::cout 
							  << std::format( "user: {}, email: {} exists", name, email ) 
							  << std::endl;
						  json_res.emplace( "error", EnumErrorCode::ErrorUserExisting );
						  beast::ostream( connection->response.body() ) << json_res.dump();

						  return true;
					  }

					  json_res.emplace( "error", 0 );
					  json_res.emplace( "uid", uid );
					  json_res.emplace( "email", email );
					  json_res.emplace( "user", name );
					  json_res.emplace( "password", pwd );
					  json_res.emplace( "verify_code", verify_code );
					  beast::ostream( connection->response.body() ) << json_res.dump();

					  return true;
				  } );
}
