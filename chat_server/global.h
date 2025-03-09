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
	Success				= 0,
	ErrorJson			= 1001,
	ErrorRpc			= 1002,
	ErrorRedisExpired	= 1003,
	ErrorVerify         = 1004,
	ErrorUserExisting   = 1005,
	ErrorPwsWrong		= 1006,
	ErrorEmailNotMatch  = 1007,
	ErrorPwdUpdateFail  = 1008,
	ErrorPwdInvaild		= 1009 
};