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
#include <GUI/GUIWindow.h>
#include <Events/EventCallback.h>
#include <Prefabs/Prefab.h>
#include <Core/Entity.h>

namespace GUI
{

  class PrefabEditor : public GUIWindow
  {
  public:
    PrefabEditor(const char* name);

    /*!*********************************************************************
    \brief
      Updates the prefab editor
    ************************************************************************/
    void Run() override;

  private:
    std::string mPrefabName;
    IGE::Assets::GUID mGUID;
    std::vector<Prefabs::SubDataId> mRemovedChildren;
    std::vector<std::pair<Prefabs::SubDataId, rttr::type>> mRemovedComponents;
    std::pair<ECS::Entity, Prefabs::Prefab::EntityMappings> mPrefabInstance;
    bool mIsEditing, mEscTriggered;

    /*!*********************************************************************
    \brief
      Switches the editor to Prefab-editing mode
    \param event
      The event to be handled
    ************************************************************************/
    EVENT_CALLBACK_DECL(OnPrefabEdit);

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
