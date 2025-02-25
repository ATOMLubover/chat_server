#pragma once

#include <fstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <string>
#include <sstream>
#include <exception>
#include <iostream>
#include <iomanip>
#include <memory>
#include <format>

namespace hatsuiki
{
	class BasicFileLogger
	{
	public:
		enum class EnumLevel
		{
			Info,
			Warning,
			Error
		};

	public:
		BasicFileLogger( const std::string& path_logfile )
			: logfile( std::make_unique<std::ofstream>( path_logfile, std::ios::app ) )
		{
			if ( !logfile->is_open() )
				throw std::runtime_error( "unable to open " + path_logfile );

			*logfile << "///// log starts /////" << std::endl;
		}
		virtual ~BasicFileLogger()
		{
			if ( logfile->is_open() )
			{
				*logfile << "///// log ends /////\n" << std::endl;
				logfile->close();
			}
			else
			{
				std::cout << "unknown error in log file" << std::endl;
			}
		}

		template<typename ...Args>
		void Log( EnumLevel level, const std::string& fmt, Args&& ...args )
		{
			if ( !logfile->is_open() )
				return;

			std::format_args fmt_args = std::make_format_args( args... );
			std::string msg = std::vformat( fmt, fmt_args );

			*logfile << std::format( "[{}][{}]: {}", GetFormatCurrTime(), GetStringLevel( level ), msg ) << std::endl;
		}

	protected:
		std::string GetFormatCurrTime() const
		{
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
			std::time_t time_now = std::chrono::system_clock::to_time_t( now );
			std::tm tm_now;
			localtime_s( &tm_now, &time_now );

			std::ostringstream oss;
			oss << std::put_time( &tm_now, "%m-%d %H:%M:%S" );
			return oss.str();
		}

		std::string GetStringLevel( EnumLevel level ) const
		{
			switch ( level )
			{
				case EnumLevel::Info:       return "INFO";
				case EnumLevel::Warning:    return "WARNING";
				case EnumLevel::Error:		return "ERROR";
				default:                    return "UNKNOWN";
			}
		}

	protected:
		std::unique_ptr<std::ofstream> logfile;
	};

	class SyncFileLogger : public BasicFileLogger
	{
	public:
		SyncFileLogger( const std::string& path_logfile )
			: BasicFileLogger( path_logfile )
			, mtx( std::make_unique<std::mutex>() )
		{ }
		~SyncFileLogger()
		{ }

		template<typename ...Args>
		void Log( EnumLevel level, const std::string& fmt, Args&& ...args )
		{
			if ( !logfile->is_open() )
				return;

			std::lock_guard<std::mutex> guard( *mtx );

			std::format_args fmt_args = std::make_format_args( args... );
			std::string msg = std::vformat( fmt, fmt_args );
			*logfile << std::format( "[{}][{}]: {}", 
									 GetFormatCurrTime(), GetStringLevel( level ), msg ) << std::endl;
		}

	protected:
		std::unique_ptr<std::mutex> mtx;
	};
} // namespace hatsuiki
