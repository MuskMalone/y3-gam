/*!*********************************************************************
\file   ThreadSafeSingleton.h
\date   5-October-2024
\brief  Thread-safe singleton template used for classes in the engine.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <memory>
#include <mutex>
#ifdef _DEBUG
#include <exception>
#include <sstream>
#include <iostream>
#endif

template <typename T>
class ThreadSafeSingleton
{
public:

  /*!*********************************************************************
  \brief
    Creates the instance of the class. Takes in a variadic list of
    constructor arguments for the class. Ensures the instance is only
    created once via std::call_once.
  ************************************************************************/
  template <typename... Args>
  static void CreateInstance(Args&&... args) {
    if (mInstance) {
#ifdef _DEBUG
      std::cout << "Singleton instance of " << typeid(T).name() << " already exists!\n";
      std::cout << "Line " << __LINE__ << " | Function: " << __FUNCTION__ << "\n";
#endif
      return;
    }

    std::call_once(mOnceFlag, [&]{
      std::unique_ptr<T> inst{ std::make_unique<T>(std::forward<Args>(args)...) };
      // splitting this into 2 lines so i can check for unintended constructor calls
      // in the default ctor
       mInstance = std::move(inst);
    });
  }

  /*!*********************************************************************
  \brief
    Returns the single instance of the class. This function uses a
    lock_guard to ensure only one thread accesses the instance at a time.
  \return
    The instance of the class
  ************************************************************************/
  static T& GetInstance() {
    std::lock_guard<std::mutex> lock(mMutex);
#ifdef _DEBUG
    if (!mInstance) {
      std::ostringstream oss{};
      oss << "Singleton instance of " << typeid(T).name() << " not initialized! Call static function CreateInstance() first! | "
        << "Line " << __LINE__ << ", Function: " << __FUNCTION__;
      std::cout << oss.str() << "\n";
      throw std::logic_error(oss.str());
    }
#endif
    return *mInstance;
  }

  /*!*********************************************************************
  \brief
    Destroys the instance of the class
  \return
    The instance of the class
  ************************************************************************/
  static void DestroyInstance() {
    mInstance.reset();
  }

protected:
  ThreadSafeSingleton() {
    if (mInstance) {
      std::ostringstream oss{};
      oss << "Singleton instance of " << typeid(T).name() << " already exists! | " 
        << "Line " << __LINE__ << ", Function: " << __FUNCTION__;
#ifdef _DEBUG
      std::cout << oss.str() << "\n";
#endif
      throw std::logic_error(oss.str());
    }
  }

  virtual ~ThreadSafeSingleton() = default;
  ThreadSafeSingleton(ThreadSafeSingleton const&) = delete;
  ThreadSafeSingleton& operator=(ThreadSafeSingleton const&) = delete;

  inline static std::unique_ptr<T> mInstance;
  inline static std::mutex mMutex;
  inline static std::once_flag mOnceFlag;  // used by std::call_once
};
