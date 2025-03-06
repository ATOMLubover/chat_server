#include "ConfigManager.h"

#include <fstream>
#include <sstream>
#include <iostream>

const std::string ConfigManager::CONFIG_PATH = "./resources/config.ini";

ConfigManager::ConfigManager()
{
	std::ifstream ifs_config( CONFIG_PATH );

	if ( !ifs_config )
	{
		std::cout << "config failed to open" << std::endl;
		return;
	}

	std::string str_config( std::istreambuf_iterator<char>( ifs_config ), std::istreambuf_iterator<char>() );
	json_config = JSON::parse( str_config );

	ifs_config.close();
}

std::string ConfigManager::GetValue( const std::string& section, const std::string& key )
{
	return json_config[ section ][ key ].get<std::string>();
}
