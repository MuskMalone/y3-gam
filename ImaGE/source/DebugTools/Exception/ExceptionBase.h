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
		virtual std::string Log(bool logToFile = true) = 0;


		/*!******************************************************************
		\brief 
			Log Exception with the source location

		\param logToFile
      bool to indicate if the msg should be written to file.

		\return 
			Logged message.
		********************************************************************/
		virtual std::string LogSource(bool logToFile = true) = 0;


	};
}

