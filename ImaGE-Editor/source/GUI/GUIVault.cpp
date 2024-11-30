#include <pch.h>
#include "GUIVault.h"
#include <Events/EventManager.h>

namespace GUI {
  void GUIVault::SetSelectedEntity(ECS::Entity entity) {
    sSelectedEntity = entity;
    QUEUE_EVENT(Events::EntitySelectedInEditor, entity);
  }
}
