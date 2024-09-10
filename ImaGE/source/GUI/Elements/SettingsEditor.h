#pragma once
#include <GUI/GUIWindow.h>
#include <Core/Settings.h>

namespace GUI
{

  class SettingsEditor : public GUIWindow
  {
  public:
    SettingsEditor(std::string const& windowName, Scene& scene);

    void Run() override;

  private:
  };
  
} // namespace GUI
