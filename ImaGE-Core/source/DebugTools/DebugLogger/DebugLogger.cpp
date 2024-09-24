/*!*********************************************************************
\file   ErrorLogger.cpp 
\author han.q@digipen.edu
\date   20-September-2024
\brief  
  Defines non template functions for logging. Constructor opens
	ile for spdlog to write to.

	Destructor will then rename the file to current time of call.
	his is so the log file is easier to find as the time is closer
	o which one would be looking for the logs.
 
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#ifndef IMGUI_DISABLE
#include "DebugLogger.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <filesystem>

using namespace Debug;


DebugLogger::DebugLogger()
{
	// Get curret system time
	std::chrono::system_clock::time_point currTime = std::chrono::system_clock::now();
	std::time_t currTime_t = std::chrono::system_clock::to_time_t(currTime);

	// Convert to format usable by put_time()
	std::tm* localTime = std::localtime(&currTime_t);

	std::stringstream ss;
	// not sure if file path should be changable by config file
	ss << std::put_time(localTime, "logs/Log_%Y.%m.%d-%H.%M.%S.txt");
	mFileName = ss.str();

	// 5mb file sinks
	auto filesink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(mFileName, 1048576 * 5, 1);

	// Default logger will log only to ostream
	mLogger = std::make_unique<spdlog::logger>("");
	// File logger will log into ostream and file
	mFileLogger = std::make_unique<spdlog::logger>("", filesink);
}

DebugLogger::~DebugLogger()
{
	// Flush logs
	mLogger->flush();
	mFileLogger.reset();

	if (std::filesystem::file_size(mFileName) == 0)
	{
		std::remove(mFileName.c_str());
	}
	else
	{
		// Rename the created file with the current time of end of application
		// This is so its easier to find the log file as it matches the time
		// someone usually looks for in recent logs

		// Get curret system time
		std::chrono::system_clock::time_point currTime = std::chrono::system_clock::now();
		std::time_t currTime_t = std::chrono::system_clock::to_time_t(currTime);

		// Convert to format usable by put_time()
		std::tm* localTime = std::localtime(&currTime_t);

		std::stringstream ss;
		// not sure if file path should be changable by config file
		ss << std::put_time(localTime, "logs/Log_%Y.%m.%d-%H.%M.%S.txt");

		std::rename(mFileName.c_str(), ss.str().c_str());
	}
}

void DebugLogger::AddDest(spdlog::sink_ptr sink)
{
	mLogger->sinks().push_back(sink);
}

void DebugLogger::AddFileDest(spdlog::sink_ptr sink)
{
	mFileLogger->sinks().push_back(sink);
}

void DebugLogger::SuppressLogMessages(bool flag)
{
	if (flag)
	{
		mLogger->set_level(spdlog::level::warn);
		mFileLogger->set_level(spdlog::level::warn);
	}
	else
	{
		mLogger->set_level(spdlog::level::trace);
		mFileLogger->set_level(spdlog::level::trace);
	}
}

std::string DebugLogger::LogInfo(std::string msg, bool logToFile)
{
#ifdef PRINTTOCOUT
	PrintToCout(msg, LVL_INFO);
#endif
	// Don't log messages
  if (logToFile)
  {
		mFileLogger->info(msg);
  }
	else
	{
		mLogger->info(msg);
	}

	return msg;
}

std::string DebugLogger::LogWarning(std::string msg, bool logToFile)
{
#ifdef PRINTTOCOUT
	PrintToCout(msg, LVL_WARN);
#endif
	if (logToFile)
	{
		mFileLogger->warn(msg);
	}
	else
	{
		mLogger->warn(msg);
	}

	return msg;
}

std::string DebugLogger::LogError(std::string msg, bool logToFile)
{
#ifdef PRINTTOCOUT
	PrintToCout(msg, LVL_ERROR);
#endif
	if (logToFile)
	{
		mFileLogger->error(msg);
	}
	else
	{
		mLogger->error(msg);
	}

	return msg;
}

std::string DebugLogger::LogCritical(std::string msg, bool logToFile)
{

#ifdef PRINTTOCOUT
	PrintToCout(msg, LVL_CRITICAL);
#endif
	if (logToFile)
	{
		mFileLogger->critical(msg);
	}
	else
	{
		mLogger->critical(msg);
	}


	return msg;
}

void DebugLogger::PrintToCout(std::string msg, EXCEPTION_LEVEL lvl)
{
	// Get the current time as a time_point
	auto now = std::chrono::system_clock::now();

	// Convert to time_t to extract the calendar time
	std::time_t now_time = std::chrono::system_clock::to_time_t(now);

	// Convert to a tm structure for local time representation
	std::tm now_tm = *std::localtime(&now_time);

	// Extract milliseconds
	auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		now.time_since_epoch()) % 1000;

	// Print the formatted time
	std::cout << '['
		<< std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S")   // Format date and time
		<< '.' << std::setfill('0') << std::setw(3)      // Add milliseconds
		<< now_ms.count()
		<< ']';
	
	switch (lvl)
	{
		case LVL_INFO:
		{
			std::cout << " [info] " << msg << "\n";
			break;
		}
		case LVL_WARN:
		{
			std::cout << " [warning] " << msg << "\n";
			break;
		}
		case LVL_ERROR:
		{
			std::cout << " [error] " << msg << "\n";
			break;
		}
		case LVL_CRITICAL:
		{
			std::cout << " [critical] " << msg << "\n";
			break;
		}
	}
}


#endif
