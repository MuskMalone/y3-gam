#include <pch.h>
#include "EntityManager.h"

Entity EntityManager::CreateEntity() {
  return Entity(m_registry.create());
}

Entity EntityManager::CreateEntityWithTag(std::string tag) {
  return Entity();
}

void EntityManager::RemoveEntity(Entity entity) {

}

Entity EntityManager::GetEntityFromTag(std::string tag) {
  return Entity();
}

void EntityManager::Reset() {
  m_registry.clear();
}