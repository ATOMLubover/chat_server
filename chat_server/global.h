#pragma once

#include "Singleton.h"

// boost libs
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

// json lib
#include <json.hpp>

// standard libs
#include <memory>
#include <functional>
#include <map>
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <format>

namespace beast = boost::beast;				// from <boost/beast.hpp>
namespace http = boost::beast::http;		// from <boost/beast/http.hpp>
namespace net = boost::asio;				// from <boost/asio.hpp>

using tcp = boost::asio::ip::tcp;			// from <boost/asio/ip/tcp.hpp>

using JSON = nlohmann::json;

enum class EnumErrorCode : std::int32_t
{
	Success,
	Error_Json = 1001,
	Error_RPC = 1002
};