#ifndef DISTRIBUTION
template <typename T>
void DebugLogger::LogInfo(std::string const& msg, bool logToFile)
{
  std::lock_guard<std::mutex> lock(mLogMutex);
#ifdef PRINTTOCOUT
	PrintToCout(typeid(T).name() + (": " + msg), LVL_INFO);
#endif

  if (!logToFile)
  {
    mLogger->info(typeid(T).name() + (": " + msg));
  }
  else
  {
    mFileLogger->info(typeid(T).name() + (": " + msg));
  }
}

template <typename T>
void DebugLogger::LogWarning(std::string const& msg, bool logToFile)
{
  std::lock_guard<std::mutex> lock(mLogMutex);
#ifdef PRINTTOCOUT
	PrintToCout(typeid(T).name() + (": " + msg), LVL_WARN);
#endif

  if (!logToFile)
  {
    mLogger->warn(typeid(T).name() + (": " + msg));
  }
  else
  {
    mFileLogger->warn(typeid(T).name() + (": " + msg));
  }
}

template <typename T>
void DebugLogger::LogError(std::string const& msg, bool logToFile)
{
  std::lock_guard<std::mutex> lock(mLogMutex);
#ifdef PRINTTOCOUT
	PrintToCout(typeid(T).name() + (": " + msg), LVL_ERROR);
#endif
  if (!logToFile)
  {
    mLogger->error(typeid(T).name() + (": " + msg));
  }
  else
  {
    mFileLogger->error(typeid(T).name() + (": " + msg));
  }
}

template <typename T>
void DebugLogger::LogCritical(std::string const& msg, bool logToFile)
{
  std::lock_guard<std::mutex> lock(mLogMutex);
#ifdef PRINTTOCOUT
	PrintToCout(typeid(T).name() + (": " + msg), LVL_CRITICAL);
#endif
  if (!logToFile)
  {
    mLogger->critical(typeid(T).name() + (": " + msg));
  }
  else
  {
    mFileLogger->critical(typeid(T).name() + (": " + msg));
  }
}
#else
template <typename T>
void DebugLogger::LogInfo([[maybe_unused]] std::string const& msg, [[maybe_unused]] bool logToFile) {}
template <typename T>
void DebugLogger::LogWarning([[maybe_unused]] std::string const& msg, [[maybe_unused]] bool logToFile) {}
template <typename T>
void DebugLogger::LogError([[maybe_unused]] std::string const& msg, [[maybe_unused]] bool logToFile) {}
template <typename T>
void DebugLogger::LogCritical([[maybe_unused]] std::string const& msg, [[maybe_unused]] bool logToFile) {}
#endif
