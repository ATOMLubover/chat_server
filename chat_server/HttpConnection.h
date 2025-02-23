#pragma once

#include "global.h"

class LogicSystem;

class HttpConnection
	: public std::enable_shared_from_this<HttpConnection>
{
	friend class LogicSystem;

public:
	HttpConnection( tcp::socket&& socket );

	void Start();

private:
	void CheckTimeout(); // check whether is connection timeout

	void HandleRequest();
	void Respond();

private:
	tcp::socket socket;

	beast::flat_buffer buf_recv{ 8 * 1024 }; // buffer set to size largger than 1500
	http::request<http::dynamic_body> request;
	http::response<http::dynamic_body> response;

	net::steady_timer timer_timeout{ socket.get_executor(), std::chrono::seconds( 20 ) }; // timeout when wait for 20 sec
};