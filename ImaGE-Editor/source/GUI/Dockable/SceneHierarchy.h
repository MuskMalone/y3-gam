#pragma once
#include "pch.h"

#include <GUI/GUIWindow.h>
#include <Core/Entity.h>
#include <Events/EventCallback.h>

// forward declaration
namespace Scenes { class SceneManager; }

namespace GUI
{

  class SceneHierarchy : public GUIWindow
  {
  public:
    SceneHierarchy(std::string const& name);

    void Run() override;

  private:
    ECS::EntityManager& mEntityManager;
    std::string mSceneName;
    ECS::Entity mRightClickedEntity; // used to hold the entity the menu was opened on
    bool mRightClickMenu, mEntityOptionsMenu, mPrefabPopup, mFirstTimePfbPopup;
    bool mEditingPrefab, lockControls;

    static constexpr char sDragDropPayload[] = "ENTITY";

    ECS::Entity CreateNewEntity() const;
    void RecurseDownHierarchy(ECS::Entity entity);
    void RunRightClickMenu() const;
    void RunEntityOptions();
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
