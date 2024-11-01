template <typename T>
Exception<T>::Exception(EXCEPTION_LEVEL lvl, std::string const& err, std::string const& loc, int line)
  : mSource{ std::move(loc) },  mLine{ line }, mLvl{ lvl }, mErrLog{ DebugLogger::GetInstance() } 
{
    std::stringstream ss;
    ss << mSource << "(Line: " << mLine << ") " << err;
    mErrMsg = ss.str(); // Directly format and set the error message
}


template <typename T>
std::string Exception<T>::LogSource(bool logToFile)
{
  switch (mLvl)
  {
  case LVL_WARN:
    return mErrLog.LogWarning<T>(mErrMsg, logToFile);
    break;
  case LVL_ERROR:
    return mErrLog.LogError<T>(mErrMsg, logToFile);
    break;
  case LVL_CRITICAL:
    return mErrLog.LogCritical<T>(mErrMsg, logToFile);
    break;
  default: 
    return mErrLog.LogInfo<T>(mErrMsg, logToFile); // Default will throw as info
    break;
  }
}

template <typename T>
std::string Exception<T>::Log(bool logToFile)
{
  switch (mLvl)
  {
  case LVL_WARN:
    return mErrLog.LogWarning(mErrMsg, logToFile);
    break;
  case LVL_ERROR:
    return mErrLog.LogError(mErrMsg, logToFile);
    break;
  case LVL_CRITICAL:
    return mErrLog.LogCritical(mErrMsg, logToFile);
    break;
  default: 
    return mErrLog.LogInfo(mErrMsg, logToFile); // Default will throw as info
    break;
  }
}