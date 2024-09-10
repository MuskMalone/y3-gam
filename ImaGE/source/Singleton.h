#pragma once

template <typename T>
class Singleton
{
public:
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
