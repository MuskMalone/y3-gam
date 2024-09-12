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

Entity EntityManager::CopyEntity(Entity entity) {
  Entity entityCopy{ EntityManager::CreateEntity() };

  // There has to be a better method...
  // But for now it is what it is...
  entityCopy.GetComponent<Component::Tag>().tag = 
    entity.GetComponent<Component::Tag>().tag + "Copy";

  if (entity.HasComponent<Component::Layer>()) {
    entityCopy.EmplaceComponent<Component::Layer>() = 
      entity.GetComponent<Component::Layer>().layerName;
  }

  if (entity.HasComponent<Component::Transform>()) {
    entityCopy.EmplaceComponent<Component::Transform>() =
      entity.GetComponent<Component::Transform>();
  }

  return entityCopy;
}

bool EntityManager::HasParent(Entity entity) {
  auto iter{ m_parent.find(entity) };
  return iter != m_parent.end();
}

bool EntityManager::HasChild(Entity entity) {
  auto iter{ m_children.find(entity) };
  return iter != m_children.end();
}

Entity EntityManager::GetEntityFromTag(std::string tag) {
  auto view{ GetAllEntitiesWithComponents<Component::Tag>() };
  for (Entity::EntityID entity : view) {
    const Component::Tag tagComp{ view.get<Component::Tag>(entity) };
    if (tagComp.tag == tag) {
      return Entity(entity);
    }
  }

  // To replace with logging
  std::cout << "No Entities have the specified Tag!\n";

  return Entity();
}

Entity EntityManager::GetParentEntity(Entity const& child) const {
  auto iter{ m_parent.find(child) };

  if (iter == m_parent.end()) {
    // To replace with logging
    std::cout << "Entity: " << child.GetTag() << " does not have a Parent!\n";   
  }

  return iter->second;
}

std::set<Entity>& EntityManager::GetChildEntity(Entity const& parent) {
  auto iter{ m_children.find(parent) };

  if (iter == m_children.end()) {
    // To replace with logging
    std::cout << "Entity: " << parent.GetTag() << " does not have a Child!\n";
  }

  return iter->second;
}

void EntityManager::SetParentEntity(Entity const& parent, Entity const& child) {
  m_parent[parent] = child;
  SetChildEntity(parent, child);
}

void EntityManager::SetChildEntity(Entity const& parent, Entity const& child) {
  m_children[parent].insert(child);
  SetParentEntity(parent, child);
}

void EntityManager::RemoveParentEntity(Entity const& child) {
  auto iter{ m_parent.find(child) };

  if (iter == m_parent.end()) {
    // To replace with logging
    std::cout << "Removing Non-existent Parent!\n";
    return;
  }

  RemoveChildEntity(m_parent[child], child);
  m_parent.erase(child);
}

void EntityManager::RemoveChildEntity(Entity const& parent, Entity const& child) {
  auto iter{ m_children[parent].find(child) };

  if (iter == m_children[parent].end()) {
    // To replace with logging
    std::cout << "Removing Non-existent Child!\n";
    return;
  }

  RemoveParentEntity(child);
  m_children[parent].erase(child);

  if (m_children[parent].empty())
    m_children.erase(parent);
}

void EntityManager::Reset() {
  m_registry.clear();
}

void EntityManager::DeleteEntity(Entity entity) {
  m_registry.destroy(entity.GetRawEnttEntityID());
}

entt::registry& EntityManager::GetRegistry() {
  return m_registry;
}