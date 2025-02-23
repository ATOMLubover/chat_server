#pragma once

#include "global.h"

class HttpConnection;
using HttpHandler = std::function<void( std::shared_ptr<HttpConnection> )>;

class LogicSystem
	: public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;

public:
	~LogicSystem() = default;

	void RegisterGet( const std::string& name_handler, HttpHandler handler );
	bool HandleGet( const std::string& url, std::shared_ptr<HttpConnection> connection );

private:
	LogicSystem();

private:
	std::map<std::string, HttpHandler> post_handlers;
	std::map<std::string, HttpHandler> get_handlers;
};

