#include <pch.h>
#include "EntityManager.h"
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
    Component::Tag& entTag = entity.EmplaceComponent<Component::Tag>();
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

  bool EntityManager::HasParent(Entity entity) const {
    if (!m_registry.valid(entity.GetRawEnttEntityID())) {
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Entity is not valid!" << std::endl;
      return false;
    }

    auto iter{ m_parent.find(entity.GetRawEnttEntityID()) };
    return iter != m_parent.end();
  }

  bool EntityManager::HasChild(Entity entity) const {
    if (!m_registry.valid(entity.GetRawEnttEntityID())) {
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Entity is not valid!" << std::endl;
      return Entity();
    }

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

    // @TODO: REPLACE WITH LOGGING SYSTEM
    std::cout << "No Entities have the specified Tag!\n";

    return Entity();
  }

  Entity EntityManager::GetParentEntity(Entity const& child) const {
    if (!m_registry.valid(child.GetRawEnttEntityID())) {
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Child is not valid!" << std::endl;
      return Entity();
    }

    auto iter{ m_parent.find(child.GetRawEnttEntityID()) };

    if (iter == m_parent.end()) {
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Entity: " << child.GetTag() << " does not have a Parent!\n";
    }

    return iter->second;
  }

  std::vector<Entity> EntityManager::GetChildEntity(Entity const& parent) {
    if (!m_registry.valid(parent.GetRawEnttEntityID())) {
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Parent is not valid!" << std::endl;
      return std::vector<Entity>();
    }

    auto iter{ m_children.find(parent.GetRawEnttEntityID()) };

    if (iter == m_children.end()) {
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Entity: " << parent.GetTag() << " does not have a Child!\n";
      return std::vector<Entity>();
    }

    std::vector<Entity> ret{};
    for (EntityID entID : iter->second) {
      ret.push_back(Entity(entID));
    }
    return ret;
  }

  void EntityManager::SetParentEntity(Entity const& parent, Entity const& child) {
    if (!m_registry.valid(parent.GetRawEnttEntityID()) || 
      !m_registry.valid(child.GetRawEnttEntityID())) {
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Parent/Child is not valid!" << std::endl;
      return;
    }

    m_parent[child.GetRawEnttEntityID()] = parent.GetRawEnttEntityID();
    m_children[parent.GetRawEnttEntityID()].insert(child.GetRawEnttEntityID());
  }

  void EntityManager::SetChildEntity(Entity const& parent, Entity const& child) {
    if (!m_registry.valid(parent.GetRawEnttEntityID()) || 
      !m_registry.valid(child.GetRawEnttEntityID())) {
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Parent/Child is not valid!" << std::endl;
      return;
    }

    m_children[parent.GetRawEnttEntityID()].insert(child.GetRawEnttEntityID());
    m_parent[child.GetRawEnttEntityID()] = parent.GetRawEnttEntityID();
  }

  std::map<EntityManager::EntityID, std::set<EntityManager::EntityID>> const& EntityManager::GetChildrenMap() const {
    return m_children;
  }

  std::map<EntityManager::EntityID, EntityManager::EntityID> const& EntityManager::GetParentMap() const {
    return m_parent;
  }

  bool EntityManager::RemoveParent(Entity const& child) {
    if (!m_registry.valid(child.GetRawEnttEntityID())) {
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Entity is not valid!" << std::endl;
      return false;
    }

    auto iter{ m_parent.find(child.GetRawEnttEntityID()) };

    if (iter == m_parent.end()) {
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Removing Non-existent Parent!\n";
      return false;
    }

    else {
      std::set<EntityID>& childList = m_children[m_parent[child.GetRawEnttEntityID()]];
      childList.erase(child.GetRawEnttEntityID());
      m_parent.erase(child.GetRawEnttEntityID());
      return true;
    }
  }

  void EntityManager::RemoveEntity(Entity const& entity) {
    if (!m_registry.valid(entity.GetRawEnttEntityID())) {
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Entity is not valid!" << std::endl;
      return;
    }

    auto iter{ m_parent.find(entity.GetRawEnttEntityID()) };

    if (iter == m_parent.end()) {
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Removing Non-existent Parent!\n";
      return;
    }

    else { // Entity has a parent, proceed to remove it from parent's child list
      std::set<EntityID> & childList = m_children[m_parent[entity.GetRawEnttEntityID()]];
      childList.erase(entity.GetRawEnttEntityID());
    }
    
    m_parent.erase(entity.GetRawEnttEntityID());
    RecursivelyRemoveParentAndChild(entity.GetRawEnttEntityID());
  }

  void EntityManager::RecursivelyRemoveParentAndChild(EntityID entity) {
    std::set<EntityID> setOfChildren = m_children[entity];
    for (EntityID child : setOfChildren) {
      m_parent.erase(child);
      RecursivelyRemoveParentAndChild(child);
    }
    m_children.erase(entity);
    DeleteEntity(entity);
  }

  void EntityManager::Reset() {
    m_registry.clear();
    m_children.clear();
    m_parent.clear();
  }

  entt::registry& EntityManager::GetRegistry() {
    return m_registry;
  }

  void EntityManager::DeleteEntity(Entity entity) {
    if (!m_registry.valid(entity.GetRawEnttEntityID())) {
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Entity is not valid!" << std::endl;
      return;
    }
    m_registry.destroy(entity.GetRawEnttEntityID());
  }
} // namespace ECS