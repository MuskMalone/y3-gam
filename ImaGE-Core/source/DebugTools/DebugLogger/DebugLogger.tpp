template <typename T>
std::string DebugLogger::LogInfo(std::string msg, bool logToFile)
{
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

  return msg;
}

template <typename T>
std::string DebugLogger::LogWarning(std::string msg, bool logToFile)
{
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

  return msg;
}

template <typename T>
std::string DebugLogger::LogError(std::string msg, bool logToFile)
{
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

  return msg;
}

template <typename T>
std::string DebugLogger::LogCritical(std::string msg, bool logToFile)
{
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

  return msg;
}