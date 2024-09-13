#include <pch.h>
#include "EntityManager.h"
#include "Component/Components.h"
#include "Entity.h"

namespace ECS {
  Entity EntityManager::CreateEntity() {
    Entity entity(m_registry.create());

    // Default all created entities to have the following components:
    entity.EmplaceComponent<Component::Tag>();
    entity.EmplaceComponent<Component::Transform>();

    return entity;
  }

  Entity EntityManager::CreateEntityWithTag(std::string const& tag) {
    Entity entity(m_registry.create());
    Component::Tag entTag = entity.EmplaceComponent<Component::Tag>(tag);
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

  bool EntityManager::HasParent(Entity entity) const {
    auto iter{ m_parent.find(entity.GetRawEnttEntityID()) };
    return iter != m_parent.end();
  }

  bool EntityManager::HasChild(Entity entity) const {
    auto iter{ m_children.find(entity.GetRawEnttEntityID()) };
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
    auto iter{ m_parent.find(child.GetRawEnttEntityID()) };

    if (iter == m_parent.end()) {
      // To replace with logging
      std::cout << "Entity: " << child.GetTag() << " does not have a Parent!\n";
    }

    return iter->second;
  }

  std::vector<Entity> EntityManager::GetChildEntity(Entity const& parent) {
    auto iter{ m_children.find(parent.GetRawEnttEntityID()) };

    if (iter == m_children.end()) {
      // To replace with logging
      std::cout << "Entity: " << parent.GetTag() << " does not have a Child!\n";
    }

    std::vector<Entity> ret{};
    for (EntityID entID : iter->second) {
      ret.emplace_back(entID);
    }

    return ret;
  }

  void EntityManager::SetParentEntity(Entity const& parent, Entity const& child) {
    m_parent[child.GetRawEnttEntityID()] = parent.GetRawEnttEntityID();
    m_children[parent.GetRawEnttEntityID()].insert(child.GetRawEnttEntityID());
  }

  void EntityManager::SetChildEntity(Entity const& parent, Entity const& child) {
    m_children[parent.GetRawEnttEntityID()].insert(child.GetRawEnttEntityID());
    m_parent[child.GetRawEnttEntityID()] = parent.GetRawEnttEntityID();
  }

  void EntityManager::RemoveParentEntity(Entity const& child) {
    auto iter{ m_parent.find(child.GetRawEnttEntityID()) };

    if (iter == m_parent.end()) {
      // To replace with logging
      std::cout << "Removing Non-existent Parent!\n";
      return;
    }

    RemoveChildEntity(m_parent[child.GetRawEnttEntityID()], child);
    m_parent.erase(child.GetRawEnttEntityID());
  }

  void EntityManager::RemoveChildEntity(Entity const& parent, Entity const& child) {
    auto iter{ m_children[parent.GetRawEnttEntityID()].find(child.GetRawEnttEntityID()) };

    if (iter == m_children[parent.GetRawEnttEntityID()].end()) {
      // To replace with logging
      std::cout << "Removing Non-existent Child!\n";
      return;
    }

    m_children[parent.GetRawEnttEntityID()].erase(child.GetRawEnttEntityID());

    if (m_children[parent.GetRawEnttEntityID()].empty())
      m_children.erase(parent.GetRawEnttEntityID());

    //RemoveParentEntity(child);
    auto iter2{ m_parent.find(child.GetRawEnttEntityID()) };

    if (iter2 == m_parent.end()) {
      // To replace with logging
      std::cout << "Removing Non-existent Parent!\n";
      return;
    }

    m_parent.erase(child.GetRawEnttEntityID());
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
} // namespace ECS