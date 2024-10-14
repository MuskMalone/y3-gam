/*!*********************************************************************
\file   ThreadSafeSingleton.h
\date   5-October-2024
\brief  Thread-safe singleton template used for classes in the engine.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <memory>
#include <mutex>

template <typename T>
class ThreadSafeSingleton
{
public:

  /*!*********************************************************************
  \brief
    Returns the single instance of the class. Creates the instance if
    none exists. This function uses a lock_guard to ensure only one
    thread accesses the instance at a time.
  \return
    The instance of the class
  ************************************************************************/
  static T& GetInstance() {
    std::lock_guard<std::mutex> lock(mMutex);
    if (!mInstance) { mInstance = std::make_shared<T>(); }

    return *mInstance;
  }

  /*!*********************************************************************
  \brief
    Destroys the instance of the class
  \return
    The instance of the class
  ************************************************************************/
  static void Destroy() {
    mInstance.reset();
  }

protected:
  ThreadSafeSingleton() = default;
  virtual ~ThreadSafeSingleton() = default;
  ThreadSafeSingleton(ThreadSafeSingleton const&) = delete;
  ThreadSafeSingleton& operator=(ThreadSafeSingleton const&) = delete;

  inline static std::unique_ptr<T> mInstance;
  inline static std::mutex mMutex;
};
