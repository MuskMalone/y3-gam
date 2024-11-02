/*!*********************************************************************
\file   Exception.h
\author han.q@digipen.edu
\date   20-September-2024
\brief
  Custom exception class with spdlog integration.
  what() is not defined
  Use Log functions below

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/

#pragma once
#include "ExceptionBase.h"
#include <string>
#include "../DebugLogger/DebugLogger.h"
#include <memory>

//#define ERRLG_FILE __FILE__
//#define ERRLG_FUNC __FUNCTION__
//#define ERRLG_LINE __LINE__

#define Msg(msg) msg, __FUNCTION__, __LINE__


  namespace Debug
  {


    template <typename T>
    class Exception : public ExceptionBase
    {
    private:
      std::string mSource{ "Unknown Source" };

      // Error line
      int mLine;
      EXCEPTION_LEVEL mLvl;

      // Singleton pointer
      DebugLogger& mErrLog;


    public:
      // Must log error with string
      Exception() = delete;

      /*!******************************************************************
      \brief 
        Creates an exception with all the values required for a custom message.
        Please throw the system in which the object throwing belongs to
        rather than individual class whenever appropriate.
        This is to avoid too many unique thrown identifies which uses
        more memory in the logger's map.

      \param lvl
        Exception level

      \param err
        Whats the error

      \param src
        Source of Error. 
          Function   or   file
        __FUNCTION__ or __FILE__

      \param line
        The line number of the error
        __LINE__
      ********************************************************************/
      Exception(EXCEPTION_LEVEL lvl, std::string const& err, std::string const& src, int line);
      // Copy ctor & copy assignment are default
       

  
      /*!******************************************************************
      \brief
        Replacement for exception what().
        interface with  LogManager to call spdlog.

      \param logToFile
        bool to indicate if the msg should be written to file.

      \return
        Logged message.
      ********************************************************************/
      std::string Log(bool logToFile = true);

      /*!******************************************************************
      \brief 
        Replacement for exception what() 
        interface with LogManager to call spdlog. 
        It also logs the object which the exception was created for. 
        
      \param[in] logToFile
        bool to indicate if the msg should be written to file.

			\return 
				Logged message.
      ********************************************************************/
      std::string LogSource(bool logToFile = true);
    };

#include "Exception.tpp"
  }

