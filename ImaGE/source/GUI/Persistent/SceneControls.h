#pragma once
#include <GUI/GUIWindow.h>
#include <Scenes/SceneManager.h>

namespace GUI
{

  class SceneControls : public GUIWindow
  {
  public:
    SceneControls(std::string const& name);

    void Run();

  private:
    Scenes::SceneManager const& mSceneManager;
  };

} // namespace GUI
