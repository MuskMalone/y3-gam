#include <pch.h>
#include "GUIVault.h"
#include <Events/EventManager.h>
#include <Scenes/SceneManager.h>

namespace GUI {
  void GUIVault::SetSelectedEntity(ECS::Entity entity) {
    sSelectedEntity = entity;
    QUEUE_EVENT(Events::EntitySelectedInEditor, entity);
  }

  EVENT_CALLBACK_DEF(GUIVault, OnSceneModified) {
    if (IGE_SCENEMGR.NoSceneSelected()) { return; }

    sSceneModified = true;
  }

  EVENT_CALLBACK_DEF(GUIVault, OnSceneStateChange) {
    switch (CAST_TO_EVENT(Events::SceneStateChange)->mNewState) {
      case Events::SceneStateChange::NEW:
      case Events::SceneStateChange::CHANGED:
        sSceneModified = false;
        break;
      default:
        break;
    }
  }

  EVENT_CALLBACK_DEF(GUIVault, OnSceneSave) {
    sSceneModified = false;
  }
}
