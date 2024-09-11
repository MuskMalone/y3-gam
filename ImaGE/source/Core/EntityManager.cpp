#include <pch.h>
#include "EntityManager.h"
#include "Entity.h"

Entity EntityManager::CreateEntity() {
  Entity entity(m_registry.create());

  // Default all created entities to have the following components:
  entity.EmplaceComponent<Component::Tag>();
  entity.EmplaceComponent<Component::Transform>();

  return entity;
}

Entity EntityManager::CreateEntityWithTag(std::string tag) {
  Entity entity(m_registry.create());
  Component::Tag entTag = entity.EmplaceComponent<Component::Tag>();
  entTag.tag = tag;
  entity.EmplaceComponent<Component::Transform>();

  return entity;
}

void EntityManager::RemoveEntity(Entity entity) {
  m_registry.destroy(entity.GetRawEnttEntityID());
}

Entity EntityManager::GetEntityFromTag(std::string tag) {
  auto view{ GetAllEntitiesWithComponents<Component::Tag>() };
  for (Entity::EntityID entity : view) {
    const Component::Tag tagComp{ view.get<Component::Tag>(entity) };
    if (tagComp.tag == tag) {
      return Entity(entity);
    }
  }
  return Entity();
}

void EntityManager::Reset() {
  m_registry.clear();
}

entt::registry& EntityManager::GetRegistry() {
  return m_registry;
}
