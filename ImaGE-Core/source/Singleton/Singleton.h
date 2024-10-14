/*!*********************************************************************
\file   Singleton.h
\date   5-October-2024
\brief  Singleton template used for classes in the engine
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <memory>

template <typename T>
class Singleton
{
public:

  /*!*********************************************************************
  \brief
    Returns the single instance of the class
  \return
    The instance of the class
  ************************************************************************/
  static T& GetInstance() {
    if (!mInstance) { mInstance = std::make_unique<T>(); }

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
  Singleton() = default;
  virtual ~Singleton() = default;
  Singleton(Singleton const&) = delete;
  Singleton& operator=(Singleton const&) = delete;

  inline static std::unique_ptr<T> mInstance;
};
