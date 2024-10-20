/*!*********************************************************************
\file   Singleton.h
\date   5-October-2024
\brief  Singleton template used for classes in the engine
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <utility>
#ifdef _DEBUG
#include <exception>
#include <sstream>
#include <iostream>
#endif

template <typename T>
class Singleton
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
#endif
      return;
    }

    std::call_once(onceFlag, [&] {
      mInstance = std::make_unique<T>(std::forward<Args>(args)...);
    });
  }

  /*!*********************************************************************
  \brief
    Returns the single instance of the class
  \return
    The instance of the class
  ************************************************************************/
  static T& GetInstance() {
#ifdef _DEBUG
    if (!mInstance) {
      std::ostringstream oss{};
      oss << "Singleton instance of " << typeid(T).name() << " not initialized! Call static function CreateInstance() first!";
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
  Singleton() = default;
  virtual ~Singleton() = default;
  Singleton(Singleton const&) = delete;
  Singleton& operator=(Singleton const&) = delete;

  inline static std::unique_ptr<T> mInstance;
};
