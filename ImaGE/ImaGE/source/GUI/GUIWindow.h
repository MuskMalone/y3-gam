#pragma once
#include <Scene.h>

namespace GUI
{
  // class that defines common interface every GUI window should have
  class GUIWindow
  {
  public:
    inline void Toggle() noexcept { m_disabled = !m_disabled; }
    inline bool IsDisabled() const noexcept { return m_disabled; }
    inline std::string GetName() const noexcept { return m_windowName; }

    virtual void Run() = 0;

  protected:
    GUIWindow(std::string windowName, Scene& scene) : m_windowName{ std::move(windowName) }, m_sceneRef { scene }, m_disabled{ false } {}

    inline std::vector<std::shared_ptr<Object>>& GetSceneObjects() noexcept { return m_sceneRef.m_objects; }

    static std::shared_ptr<Object> m_selectedObj;

    std::string const m_windowName;
    Scene& m_sceneRef;
    bool m_disabled;
  };

};  // namespace GUI
