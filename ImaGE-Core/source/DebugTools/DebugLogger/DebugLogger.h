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
#include <spdlog/spdlog.h>
#include <Singleton/ThreadSafeSingleton.h>
#include <DebugTools/Exception/ExceptionBase.h>
#include <memory>
#include <map>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#ifdef _DEBUG
#define LOG_TO_FILE true
#else
#define LOG_TO_FILE false
#endif

using namespace spdlog;
//#define PRINTTOCOUT
namespace Debug
{
  class DebugLogger : public ThreadSafeSingleton<DebugLogger>
  {
//#ifndef IMGUI_DISABLE
  private:
    // Alias
    using LoggerPtr = std::unique_ptr<spdlog::logger>;

    LoggerPtr mLogger;
    LoggerPtr mFileLogger;

    std::string mFileName;

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


    void PrintToCout(std::string msg, EXCEPTION_LEVEL lvl);

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
    std::string LogInfo(std::string msg, bool logToFile = LOG_TO_FILE);


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
    std::string LogWarning(std::string msg, bool logToFile = LOG_TO_FILE);

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
    std::string LogError(std::string msg, bool logToFile = LOG_TO_FILE);

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
    std::string LogCritical(std::string msg, bool logToFile = LOG_TO_FILE);



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
    std::string LogInfo(std::string msg, bool logToFile = LOG_TO_FILE);


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
    std::string LogWarning(std::string msg, bool logToFile = LOG_TO_FILE);



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
    std::string LogError(std::string msg, bool logToFile = LOG_TO_FILE);

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
    std::string LogCritical(std::string msg, bool logToFile = LOG_TO_FILE);
  };

#include "DebugLogger.tpp"
//#else
  /*
  public:
    template <typename T> std::string LogMessage(std::string, bool = true) { return {}; }
    std::string LogMessage(std::string, bool = true) { return {}; }
    template <typename T> std::string LogWarning(std::string, bool = true) { return {}; }
    std::string LogWarning(std::string, bool = true) { return {}; }
    template <typename T> std::string LogError(std::string, bool = true) { return {}; }
    std::string LogError(std::string, bool = true) { return {}; }
    template <typename T> std::string LogCritical(std::string, bool = true) { return {}; }
    std::string LogCritical(std::string, bool = true) { return {}; }
    */
//};
//#endif
  }

