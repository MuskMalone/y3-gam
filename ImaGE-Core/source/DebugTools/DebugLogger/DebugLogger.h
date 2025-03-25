/*!*********************************************************************
\file   ErrorLogger.h
\author han.q@digipen.edu
\date   20-September-2024
\brief
  Singleton error logger holding spdlog objects for logging.
  The class is not static as it is required to hold data ie.
  streams for output and to dump all logs to a file by default

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <Singleton/ThreadSafeSingleton.h>
#include <DebugTools/Exception/ExceptionBase.h>
#include <memory>
#include <mutex>

#ifdef DISTRIBUTION
#define LOG_TO_FILE false
#else
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#define LOG_TO_FILE true
#endif

#define IGE_DBGLOGGER Debug::DebugLogger::GetInstance()

//#define PRINTTOCOUT
namespace Debug
{
  class DebugLogger : public ThreadSafeSingleton<DebugLogger>
  {
  private:
    // Alias
#ifndef DISTRIBUTION
    using LoggerPtr = std::unique_ptr<spdlog::logger>;

    LoggerPtr mLogger;
    LoggerPtr mFileLogger;

    std::string mFileName;
    std::mutex mLogMutex;
#endif

  public:

    /*!******************************************************************
    \brief
      Setups all the intial sink and loggers to different streams.
    ********************************************************************/
    DebugLogger();
    /*!******************************************************************
    \brief
      Flushes all loggers into their streams and renames output file
      to match the current time of exit.
    ********************************************************************/
    ~DebugLogger();

#ifndef DISTRIBUTION
    /*!******************************************************************
    \brief
      Adds a sink/Destination to the ostream logger.

    \param dest
      Sink to add.
    ********************************************************************/
    void AddDest(spdlog::sink_ptr dest);

    /*!******************************************************************
    \brief
      Adds a sink/Destination to the file logger.

    \param dest
      Sink to add.
    ********************************************************************/
    void AddFileDest(spdlog::sink_ptr dest);

    /*!******************************************************************
    \brief
      Causes spdlog to ignore all logs of level log.

    \param[in] flag
      Flag to indicate supression.
    ********************************************************************/
    void SuppressLogMessages(bool flag);

    void PrintToCout(std::string const& msg, EXCEPTION_LEVEL lvl);
#endif

    /*****************************************************************/
    /*                 Logging with Source location                  */
    /*****************************************************************/


    /*!******************************************************************
    \brief
      Logs a message with Logger name being T, with custom input message.
      The log level of this function is info.

    \param msg
      Custom message to log.

    \param logToFile
      Flag to indicate if this msg should be written to file.

    \return
      Logged message.
    ********************************************************************/
    template <typename T>
    void LogInfo(std::string const& msg, bool logToFile = LOG_TO_FILE);


    /*!******************************************************************
    \brief
      Logs a message with Logger name being T, with custom input message.
      The log level of this function is warning.

    \param msg
      Custom message to log.

    \param logToFile
      Flag to indicate if this msg should be written to file.

    \return
      Logged message.
    ********************************************************************/
    template <typename T>
    void LogWarning(std::string const& msg, bool logToFile = LOG_TO_FILE);

    /*!******************************************************************
    \brief
      Logs a message with Logger name being T, with custom input message.
      The log level of this function is error.

    \param msg
      Custom message to log.

    \param logToFile
      Flag to indicate if this msg should be written to file.

    \return
      Logged message.
    ********************************************************************/
    template <typename T>
    void LogError(std::string const& msg, bool logToFile = LOG_TO_FILE);

    /*!******************************************************************
    \brief
      Logs a message with Logger name being T, with custom input message.
      The log level of this function is critical.

    \param msg
      Custom message to log.

    \param logToFile
      Flag to indicate if this msg should be written to file.

    \return
      Logged message.
    ********************************************************************/
    template <typename T>
    void LogCritical(std::string const& msg, bool logToFile = LOG_TO_FILE);



    /*****************************************************************/
    /*              Logging without Source location                  */
    /*****************************************************************/

    /*!******************************************************************
    \brief
      Logs a nameless message with custom input message.
      The log level of this function is info.

    \param msg
      Custom message to log.

    \param logToFile
      Flag to indicate if this msg should be written to file.

    \return
      Logged message.
    ********************************************************************/
    void LogInfo(std::string const& msg, bool logToFile = LOG_TO_FILE);


    /*!******************************************************************
    \brief
      Logs a nameless message with custom input message.
      The log level of this function is warning.

    \param msg
      Custom message to log.

    \param logToFile
      Flag to indicate if this msg should be written to file.

    \return
      Logged message.
    ********************************************************************/
    void LogWarning(std::string const& msg, bool logToFile = LOG_TO_FILE);



    /*!******************************************************************
    \brief
      Logs a nameless message with custom input message.
      The log level of this function is error.

    \param msg
      Custom message to log.

    \param logToFile
      Flag to indicate if this msg should be written to file.

    \return
      Logged message.
    ********************************************************************/
    void LogError(std::string const& msg, bool logToFile = LOG_TO_FILE);

    /*!******************************************************************
    \brief
      Logs a nameless message with custom input message.
      The log level of this function is critical.

    \param msg
      Custom message to log.

    \param logToFile
      Flag to indicate if this msg should be written to file.

    \return
      Logged message.
    ********************************************************************/
    void LogCritical(std::string const& msg, bool logToFile = LOG_TO_FILE);
  };

#include "DebugLogger.tpp"
  }

