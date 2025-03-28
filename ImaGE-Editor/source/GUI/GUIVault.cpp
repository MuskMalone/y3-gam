#include <pch.h>
#include "GUIVault.h"
#include <Events/EventManager.h>
#include <Scenes/SceneManager.h>

namespace GUI {
  void GUIVault::SetSelectedEntity(ECS::Entity entity) {
    sPreviousEntity = sSelectedEntity;
    sSelectedEntity = entity;
    QUEUE_EVENT(Events::EntitySelectedInEditor, entity);
  }


  void GUIVault::SetSelectedFile(std::filesystem::path const& file) {
    mSelectedFile = file;
    sPreviousEntity = sSelectedEntity;
    sSelectedEntity = {};
  }

  EVENT_CALLBACK_DEF(GUIVault, OnSceneModified) {
    if (IGE_SCENEMGR.NoSceneSelected()) { return; }

    sSceneModified = true;
  }

  EVENT_CALLBACK_DEF(GUIVault, OnSceneStateChange) {
    auto sscEvent{ CAST_TO_EVENT(Events::SceneStateChange) };
    switch (sscEvent->mNewState) {
      case Events::SceneStateChange::NEW:
      case Events::SceneStateChange::CHANGED:
        if (!sscEvent->mIsReload) {
          sSceneModified = false;
        }
        break;
      default:
        break;
    }
  }

  EVENT_CALLBACK_DEF(GUIVault, OnSceneSave) {
    sSceneModified = false;
  }
}
