#pragma once

#include "global.h"

class Server
	: public std::enable_shared_from_this<Server> // CRTP
{
public:
	Server( net::io_context& ioc, unsigned short port );

	void Start();

private:
	net::io_context& ioc;			// bottom dispatcher, ref in order to keep uniqueness

	tcp::acceptor acceptor;
	tcp::socket socket;
};