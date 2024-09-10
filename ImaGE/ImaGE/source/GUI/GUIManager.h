#pragma once
#include <vector>
#include <memory>
#include <GUI/GUIWindow.h>
#include <Scene.h>

namespace GUI
{
  class GUIManager
  {
  public:
    static void Init(Scene& scene);
    static void UpdateGUI();

  private:
    static bool m_isPopupShown;

    static std::vector<std::unique_ptr<GUIWindow>> m_windows;
    
    static void UpdatePopUpMenu();
  };

} // namespace GUI
