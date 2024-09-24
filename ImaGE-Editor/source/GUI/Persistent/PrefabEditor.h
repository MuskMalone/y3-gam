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
#include "pch.h"
#include <GUI/GUIWindow.h>
#include <Events/EventCallback.h>
#include <Prefabs/VariantPrefab.h>

namespace GUI
{

  class PrefabEditor : public GUIWindow
  {
  public:
    PrefabEditor(std::string const& name);

    void Run() override;

  private:
    std::string mPrefabName, mPrefabPath;
    std::vector<Prefabs::PrefabSubData::SubDataId> mRemovedChildren;
    std::vector<std::pair<Prefabs::PrefabSubData::SubDataId, rttr::type>> mRemovedComponents;
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
      This function is called after a prefab has been modified. It compares
      the current set of components with the previous copy and keeps track
      of any component deletions. These components will then be recorded
      to update any existing instances in future.
    ************************************************************************/
    void CheckForDeletions();

    /*!*********************************************************************
    \brief
      Clears and resets all variables of the prefab editor
    ************************************************************************/
    void ResetPrefabEditor();
  };

} // namespace GUI

#endif  // IMGUI_DISABLE
