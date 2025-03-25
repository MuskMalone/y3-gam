/*!*********************************************************************
\file   ExceptionBase.h
\author han.q@digipen.edu
\date   20-September-2024
\brief  
  Base exception class to allow for generic catch.
 
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once

#include <exception>
#include <string>

namespace Debug
{
	enum EXCEPTION_LEVEL
	{
		LVL_INFO = 0,     // Minor issues
		LVL_WARN,         // Potential issues 
		LVL_ERROR,        // Errors that can crash the program.
		LVL_CRITICAL,     // Errors that will crash the program.
	};
	class ExceptionBase : public std::exception
	{
	public:

		/*!******************************************************************
		\brief
			Logs the exception.

		\param logToFile
			DebugLogger

		\return
			Logged message.
		********************************************************************/
		virtual void Log(bool logToFile = true) = 0;


		/*!******************************************************************
		\brief 
			Log Exception with the source location

		\param logToFile
      bool to indicate if the msg should be written to file.

		\return 
			Logged message.
		********************************************************************/
		virtual void LogSource(bool logToFile = true) = 0;

		std::string const& ErrMsg() const noexcept { return mErrMsg; }

	protected:
		std::string mErrMsg{ "Unknown Error" };
	};
}

