#pragma once
#include <Scene.h>

namespace GUI
{
  // class that defines common interface every GUI window should have
  class GUIWindow
  {
  public:
    inline void Toggle() noexcept { m_active = !m_active; }
    inline bool IsActive() const noexcept { return m_active; }
    inline std::string GetName() const noexcept { return m_windowName; }

    virtual void Run() = 0;

  protected:
    GUIWindow(std::string windowName) : m_windowName{ std::move(windowName) }, m_active{ true } {}

    std::string const m_windowName;
    bool m_active;
  };

};  // namespace GUI
