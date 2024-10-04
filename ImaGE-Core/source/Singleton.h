/*!*********************************************************************
\file   Singleton.h
\date   5-October-2024
\brief  Singleton template used for classes in the engine
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once

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
  static T& GetInstance()
  {
    static T inst;
    return inst;
  }

protected:
  Singleton() = default;
  Singleton(Singleton const&) = delete;
  virtual ~Singleton() = default;
  Singleton& operator=(Singleton const&) = delete;
};
