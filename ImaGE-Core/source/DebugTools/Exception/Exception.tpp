template <typename T>
Exception<T>::Exception(EXCEPTION_LEVEL lvl, std::string const& err, std::string const& loc, int line)
  : mSource{ std::move(loc) },  mLine{ line }, mLvl{ lvl }, mErrLog{ DebugLogger::GetInstance() } 
{
    std::stringstream ss;
    ss << mSource << "(Line: " << mLine << ") " << err;
    mErrMsg = ss.str(); // Directly format and set the error message
}


template <typename T>
void Exception<T>::LogSource(bool logToFile)
{
  switch (mLvl)
  {
  case LVL_WARN:
    mErrLog.LogWarning<T>(mErrMsg, logToFile);
    break;
  case LVL_ERROR:
    mErrLog.LogError<T>(mErrMsg, logToFile);
    break;
  case LVL_CRITICAL:
    mErrLog.LogCritical<T>(mErrMsg, logToFile);
    break;
  default: 
    mErrLog.LogInfo<T>(mErrMsg, logToFile); // Default will throw as info
    break;
  }
}

template <typename T>
void Exception<T>::Log(bool logToFile)
{
  switch (mLvl)
  {
  case LVL_WARN:
    mErrLog.LogWarning(mErrMsg, logToFile);
    break;
  case LVL_ERROR:
    mErrLog.LogError(mErrMsg, logToFile);
    break;
  case LVL_CRITICAL:
    mErrLog.LogCritical(mErrMsg, logToFile);
    break;
  default: 
    mErrLog.LogInfo(mErrMsg, logToFile); // Default will throw as info
    break;
  }
}