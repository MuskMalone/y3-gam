#pragma once
#ifndef IMGUI_DISABLE
#include <GUI/GUIWindow.h>
#include <Scenes/SceneManager.h>

namespace GUI
{

  class SceneControls : public GUIWindow
  {
  public:
    SceneControls(std::string const& name);

    void Run() override;

  private:
    Scenes::SceneManager const& mSceneManager;
  };

} // namespace GUI

#endif  // IMGUI_DISABLE
