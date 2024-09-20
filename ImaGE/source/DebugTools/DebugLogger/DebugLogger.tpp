template <typename T>
std::string DebugLogger::LogInfo(std::string msg, bool logToFile)
{
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