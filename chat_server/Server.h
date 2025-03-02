#pragma once

#include "global.h"

class Server
	: public std::enable_shared_from_this<Server> // CRTP
{
public:
	Server( net::io_context& ioc, unsigned short port );

	void Start();

private:
	net::io_context& ioc;	 // ÓÃÓÚ Start() µÄ io_context

	tcp::acceptor acceptor;
};