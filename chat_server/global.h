#pragma once

#include "FileLogger.hpp"
#include "Singleton.h"

// boost libs
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

// standard libs
#include <memory>
#include <functional>
#include <map>
#include <string>

namespace beast = boost::beast;				// from <boost/beast.hpp>
namespace http = boost::beast::http;		// from <boost/beast/http.hpp>
namespace net = boost::asio;				// from <boost/asio.hpp>

using tcp = boost::asio::ip::tcp;			// from <boost/asio/ip/tcp.hpp>

using Logger = hatsuiki::SyncFileLogger;	// custom file sync logger class

extern hatsuiki::SyncFileLogger logger;		// my sync file logger