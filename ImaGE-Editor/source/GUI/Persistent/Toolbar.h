#pragma once
#ifndef IMGUI_DISABLE
#include <GUI/GUIWindow.h>
#include <vector>
#include <memory>
#include <Events/EventCallback.h>

namespace GUI
{

  class Toolbar : public GUIWindow
  {
  public:
    Toolbar(std::string const& name, std::vector<std::unique_ptr<GUIWindow>> const& windowsRef);

    void Run() override;

  private:
    std::vector<std::unique_ptr<GUIWindow>> const& mWindowsRef;  // to manage active states
    bool mScenePopup, mPrefabPopup;
    bool mDisableAll, mAllowCreationOnly;

    void RunNewScenePopup();
    void RunNewPrefabPopup();

    /*!*********************************************************************
    \brief
      Handles the events the AssetBrowser subscribed to.

      SCENE_STATE_CHANGE
        - Disables certain menu options based on the current scene state
    \param event
      The event to handle
    ************************************************************************/
    EVENT_CALLBACK_DECL(HandleEvent);
  };

} // namespace GUI

#endif  // IMGUI_DISABLE
