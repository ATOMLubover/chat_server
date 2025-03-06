#pragma once

#include "Singleton.h"

#include <json.hpp>

class ConfigManager
	: public Singleton<ConfigManager>
{
	friend class Singleton<ConfigManager>;

	using JSON = nlohmann::json;

public:
	ConfigManager( const ConfigManager& ) = delete;
	ConfigManager& operator=( const ConfigManager& rhs ) = delete;
	~ConfigManager() = default;

	std::string GetValue( const std::string& section, const std::string& key );

private:
	ConfigManager();

private:
	static const std::string CONFIG_PATH;

	JSON json_config;
};