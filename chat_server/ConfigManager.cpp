#include "ConfigManager.h"

#include <fstream>
#include <sstream>
#include <iostream>

const std::string ConfigManager::CONFIG_PATH = "./resources/config.json";

ConfigManager::ConfigManager()
{
	std::ifstream ifs_config( CONFIG_PATH );

	if ( !ifs_config )
	{
		std::cout << "config failed to open" << std::endl;
		return;
	}

	std::string str_config = std::string( std::istreambuf_iterator<char>( ifs_config ), std::istreambuf_iterator<char>() );
	json_config = JSON::parse( str_config );

	std::cout
		<< "config loaded:" << std::endl
		<< json_config.dump( 4 ) << std::endl;

	ifs_config.close();
}

std::string ConfigManager::GetValue( const std::string& section, const std::string& key )
{
	return json_config[ section ][ key ].get<std::string>();
}
