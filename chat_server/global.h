#pragma once

#include "FileLogger.hpp"
#include "Singleton.h"

// boost libs
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/locale.hpp>
#include <boost/network/uri.hpp>
#include <boost/network/uri/decode.hpp>
#include <boost/network/uri/encode.hpp>

// standard libs
#include <memory>
#include <functional>
#include <map>
#include <string>
#include <iomanip>
#include <sstream>
#include <cctype>

namespace beast = boost::beast;				// from <boost/beast.hpp>
namespace http = boost::beast::http;		// from <boost/beast/http.hpp>
namespace net = boost::asio;				// from <boost/asio.hpp>
namespace uri = boost::network::uri;		// from <boost/network/uri/>

using tcp = boost::asio::ip::tcp;			// from <boost/asio/ip/tcp.hpp>

using Logger = hatsuiki::SyncFileLogger;	// custom file sync logger class

extern hatsuiki::SyncFileLogger logger;		// my sync file logger