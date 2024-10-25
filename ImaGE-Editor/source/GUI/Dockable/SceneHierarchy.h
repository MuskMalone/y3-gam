/*!*********************************************************************
\file   SceneHierarchy.h
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Class encapsulating functions to run the scene hierarchy
        window of the editor. Displays the list of entities currently
        in the scene along with their position in the hierarchy.
        Features right-click options as well as parenting of entities.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <GUI/GUIWindow.h>
#include <Core/Entity.h>
#include <Events/EventCallback.h>
#include <ImGui/imgui.h>

// forward declaration
namespace Scenes { class SceneManager; }

namespace GUI
{

  class SceneHierarchy : public GUIWindow
  {
  public:
    SceneHierarchy(const char* name);

    /*!*********************************************************************
    \brief
      Runs the scene hierarchy window
    ************************************************************************/
    void Run() override;

  private:
    ECS::EntityManager& mEntityManager;
    std::string mSceneName;
    ECS::Entity mRightClickedEntity; // used to hold the entity the menu was opened on
    bool mRightClickMenu, mEntityOptionsMenu, mPrefabPopup, mFirstTimePfbPopup;
    bool mEditingPrefab, mLockControls, mSceneModified;

    static inline constexpr char sDragDropPayload[] = "ENTITY";
    static inline constexpr ImU32 sEntityHighlightCol{ IM_COL32(253, 208, 23, 255) };
    static inline constexpr float sTimeBeforeRename = 0.5f; // time after a click before triggerring an entity rename

    /*!*********************************************************************
    \brief
      Helper function to create a new entity
    \return
      The entity created
    ************************************************************************/
    ECS::Entity CreateNewEntity() const;

    /*!*********************************************************************
    \brief
      Recursive function to display the list of entities in a tree node
      and showing the hierarchy accordingly
    \param entity
      The current entity being displayed
    ************************************************************************/
    void RecurseDownHierarchy(ECS::Entity entity);

    /*!*********************************************************************
    \brief
      Checks for input and handles them accordingly
    \param entity
      The current entity
    ************************************************************************/
    void ProcessInput(ECS::Entity entity);

    /*!*********************************************************************
    \brief
      Runs the right-click menu when it is triggered
    ************************************************************************/
    void RunRightClickMenu() const;

    /*!*********************************************************************
    \brief
      Displays the contents of the right-click menu
    ************************************************************************/
    void RunEntityOptions();

    /*!*********************************************************************
    \brief
      Renders the popup for when a prefab is created from an existing
      entity
    ************************************************************************/
    void RunPrefabPopup();

    /*!*********************************************************************
    \brief
      This function handles the corresponding events the PrefabEditor
      subscribed to.

      SCENE_STATE_CHANGE
        - Disables certain menu options based on the current scene state
      SCENE_MODIFIED
        - Adds an asterik (*) to the scene name to indicate changes have
          been made
      EDIT_PREFAB
        - Switches the hierarchy to Prefab-editing mode

    \param event
      The event to be handled
    ************************************************************************/
    EVENT_CALLBACK_DECL(HandleEvent);
  };

} // namespace GUI