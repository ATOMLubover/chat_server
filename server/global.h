#pragma once


// 全局头文件
// 包含所有常用的工具头文件和透传的枚举类等

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
#include <iomanip>
#include <sstream>
#include <cctype>
#include <format>

namespace beast = boost::beast;				// from <boost/beast.hpp>
namespace http = boost::beast::http;		// from <boost/beast/http.hpp>
namespace net = boost::asio;				// from <boost/asio.hpp>

using tcp = boost::asio::ip::tcp;			// from <boost/asio/ip/tcp.hpp>

using JSON = nlohmann::json;				// from <json.hpp>