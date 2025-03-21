﻿#pragma once

#include "global.h"

#include <cstdint>

class LogicSystem;

class HttpConnection
	: public std::enable_shared_from_this<HttpConnection>
{
	friend class LogicSystem;

public:
	HttpConnection( boost::asio::io_context& ioc );

	void Start();

	tcp::socket& GetSocket() { return socket; }

private:
	void CheckTimeout(); // check whether is connection timeout

	void HandleRequest();
	void WriteResponse();

	std::string EncodeUrl( const std::string& raw );
	std::string DecodeUrl( const std::string& url );

	void PreparseGetParams();

private:
	tcp::socket socket;

	beast::flat_buffer buf_recv{ 8 * 1024 }; // buffer set to size largger than 1500
	http::request<http::dynamic_body> request;
	http::response<http::dynamic_body> response;

	net::steady_timer timer_timeout{ socket.get_executor(), std::chrono::seconds( 20 ) }; // timeout when wait for 20 sec

	std::string get_url; // route of entire url 
	std::map<std::string, std::string> get_params;
};