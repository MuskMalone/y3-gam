#pragma once
#include <GUI/GUIWindow.h>
#include <vector>
#include <memory>
#include <Scenes/SceneManager.h>

namespace GUI
{

  class Toolbar : public GUIWindow
  {
  public:
    Toolbar(std::string const& name, std::vector<std::unique_ptr<GUIWindow>> const& windowsRef);

    void Run() override;

  private:
    std::vector<std::unique_ptr<GUIWindow>> const& mWindowsRef;  // to manage active states
    Scenes::SceneManager& mSceneManager;
    bool mScenePopup, mPrefabPopup;

    void RunNewScenePopup();
    void RunNewPrefabPopup();
  };

} // namespace GUI
