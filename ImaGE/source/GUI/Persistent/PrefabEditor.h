/*!*********************************************************************
\file   PrefabEditor.h
\author chengen.lau\@digipen.edu
\date   14-September-2024
\brief  Contains the class encapsulating functions for the prefab
        editor feature. Different windows in the engine reference
        this class to determine which mode is currently running
        (scene or prefab editor).

        Why is this a persistent element (non-dockable) ?
          - This class tags upon the inspector and scene hierarchy
            during prefab editing mode. It only tracks the changes
            made and updates the PrefabManager accordingly.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#ifndef IMGUI_DISABLE
#include <GUI/GUIWindow.h>
#include <Events/EventCallback.h>
#include <Prefabs/Prefab.h>
#include <Core/Entity.h>

namespace GUI
{

  class PrefabEditor : public GUIWindow
  {
  public:
    PrefabEditor(std::string const& name);

    void Run() override;

  private:
    std::string mPrefabName, mPrefabPath;
    std::vector<Prefabs::SubDataId> mRemovedChildren;
    std::vector<std::pair<Prefabs::SubDataId, rttr::type>> mRemovedComponents;
    ECS::Entity mPrefabInstance;
    bool mIsEditing, mEscTriggered;

    /*!*********************************************************************
    \brief
      This function handles the corresponding events the PrefabEditor
      subscribed to.

      SCENE_STATE_CHANGE
        - Disables certain menu options based on the current scene state
      EDIT_PREFAB
        - Switches the editor to Prefab-editing mode

    \param event
      The event to be handled
    ************************************************************************/
    EVENT_CALLBACK_DECL(HandleEvent);

    /*!*********************************************************************
    \brief
      Renders the popup when returning back to the scene from the prefab
      editor.
    ************************************************************************/
    void BackToScenePopup();

    /*!*********************************************************************
    \brief
      Clears and resets all variables of the prefab editor
    ************************************************************************/
    void ResetPrefabEditor();
  };

} // namespace GUI

#endif  // IMGUI_DISABLE
