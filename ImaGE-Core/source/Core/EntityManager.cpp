#include <pch.h>
#include "EntityManager.h"
#include "Entity.h"
#include <Core/Components/Components.h>
#include <Events/EventManager.h>

namespace ECS {
  Entity EntityManager::CreateEntity() {
    Entity entity{ mRegistry.create() };

    // Default all created entities to have the following components:
    entity.EmplaceComponent<Component::Tag>();
    entity.EmplaceComponent<Component::Transform>();
    entity.EmplaceComponent<Component::Layer>();
    entity.SetLayer("Default");

    return entity;
  }
  
  Entity EntityManager::CreateEntityWithID([[maybe_unused]] ECSKey key, EntityID entityID) {
    Entity entity{ mRegistry.create(entityID) };

    // Default all created entities to have the following components:
    entity.EmplaceComponent<Component::Tag>();
    entity.EmplaceComponent<Component::Transform>();
    entity.EmplaceComponent<Component::Layer>();
    entity.SetLayer("Default");

    return entity;
  }

  Entity EntityManager::CreateEntityWithTag(std::string const& tag) {
    Entity entity(mRegistry.create());

    // Default all created entities to have the following components:
    entity.EmplaceComponent<Component::Tag>(tag);
    entity.EmplaceComponent<Component::Transform>();
    entity.EmplaceComponent<Component::Layer>();
    entity.SetLayer("Default");

    return entity;
  }

  Entity EntityManager::CopyEntity(Entity entity) {
    Entity entityCopy{ EntityManager::CreateEntity() };

    // There has to be a better method...
    // But for now it is what it is...
    entityCopy.GetComponent<Component::Tag>().tag =
      entity.GetComponent<Component::Tag>().tag + " (Copy)";

    if (entity.HasComponent<Component::Layer>()) {
      entityCopy.EmplaceComponent<Component::Layer>() =
        entity.GetComponent<Component::Layer>().name;
    }

    if (entity.HasComponent<Component::Transform>()) {
      entityCopy.EmplaceComponent<Component::Transform>() =
        entity.GetComponent<Component::Transform>();
    }

    return entityCopy;
  }

  bool EntityManager::HasParent(Entity entity) const {
    if (!mRegistry.valid(entity.GetRawEnttEntityID())) {
      Debug::DebugLogger::GetInstance().LogError("[EntityManager] Entity is not valid!");
      return false;
    }

    auto iter{ mParent.find(entity.GetRawEnttEntityID()) };
    return iter != mParent.end();
  }

  bool EntityManager::HasChild(Entity entity) const {
    if (!mRegistry.valid(entity.GetRawEnttEntityID())) {
      Debug::DebugLogger::GetInstance().LogError("[EntityManager] Entity is not valid!");
      return Entity();
    }

    auto iter{ mChildren.find(entity.GetRawEnttEntityID()) };
    // added 2nd check to ensure there are no children
    // also removes the need to erase from map when deleting
    return iter != mChildren.end() && !iter->second.empty();
  }

  bool EntityManager::IsValidEntity(Entity entity) const {
     return mRegistry.valid(entity.GetRawEnttEntityID());
  }

  Entity EntityManager::GetEntityFromTag(std::string tag) {
    auto view{ GetAllEntitiesWithComponents<Component::Tag>() };
    for (Entity::EntityID entity : view) {
      const Component::Tag tagComp{ view.get<Component::Tag>(entity) };
      if (tagComp.tag == tag) {
        return Entity(entity);
      }
    }

    Debug::DebugLogger::GetInstance().LogError("[EntityManager] No Entities have the specified Tag!");
    return Entity();
  }

  Entity EntityManager::GetParentEntity(Entity const& child) const {
    if (!mRegistry.valid(child.GetRawEnttEntityID())) {
      Debug::DebugLogger::GetInstance().LogError("[EntityManager] Child is not valid!");
      return Entity();
    }

    auto iter{ mParent.find(child.GetRawEnttEntityID()) };

    if (iter == mParent.end()) {
      Debug::DebugLogger::GetInstance().LogError("[EntityManager] Entity: " + child.GetTag() + " does not have a Parent!");
    }

    return iter->second;
  }

  std::vector<Entity> EntityManager::GetChildEntity(Entity const& parent) {
    if (!mRegistry.valid(parent.GetRawEnttEntityID())) {
      Debug::DebugLogger::GetInstance().LogError("[EntityManager] Parent is not valid!");
      return std::vector<Entity>();
    }

    auto iter{ mChildren.find(parent.GetRawEnttEntityID()) };

    if (iter == mChildren.end()) {
      Debug::DebugLogger::GetInstance().LogError("[EntityManager] Entity: " + parent.GetTag() + " does not have a Child!");
      return std::vector<Entity>();
    }

    std::vector<Entity> ret{};
    for (EntityID entID : iter->second) {
      ret.push_back(Entity(entID));
    }
    return ret;
  }

  // Alternative to the GetChildEntity function that gets the children of children, and etcs
  std::vector<Entity> EntityManager::GetChildEntityRecursively(Entity const& parent) {
    if (!mRegistry.valid(parent.GetRawEnttEntityID())) {
      Debug::DebugLogger::GetInstance().LogError("[EntityManager] Parent is not valid!");
      return std::vector<Entity>();
    }

    auto iter{ mChildren.find(parent.GetRawEnttEntityID()) };

    if (iter == mChildren.end()) {
      return std::vector<Entity>();
    }

    std::vector<Entity> totalChildren;
    for (EntityID childID : iter->second) {
      Entity childEntity(childID);
      totalChildren.push_back(childEntity);

      std::vector<Entity> childDescendants = GetChildEntityRecursively(childEntity);
      totalChildren.insert(totalChildren.end(), childDescendants.begin(), childDescendants.end());
    }

    return totalChildren;
  }

  std::unordered_map<EntityManager::EntityID, std::set<EntityManager::EntityID>> const& EntityManager::GetChildrenMap() const {
      return mChildren;
  }

  std::unordered_map<EntityManager::EntityID, EntityManager::EntityID> const& EntityManager::GetParentMap() const {
      return mParent;
  }

  void EntityManager::SetParentEntity(Entity const& parent, Entity const& child) {
    if (!mRegistry.valid(parent.GetRawEnttEntityID()) || 
      !mRegistry.valid(child.GetRawEnttEntityID())) {
      Debug::DebugLogger::GetInstance().LogError("[EntityManager] Parent(" + std::to_string(parent.GetEntityID())
        + ")/Child(" + std::to_string(child.GetEntityID()) + ") is not valid!");
      return;
    }

    mParent[child.GetRawEnttEntityID()] = parent.GetRawEnttEntityID();
    mChildren[parent.GetRawEnttEntityID()].insert(child.GetRawEnttEntityID());
  }

  void EntityManager::SetChildEntity(Entity const& parent, Entity const& child) {
    if (!mRegistry.valid(parent.GetRawEnttEntityID()) || 
      !mRegistry.valid(child.GetRawEnttEntityID())) {
      Debug::DebugLogger::GetInstance().LogError("[EntityManager] Parent(" + std::to_string(parent.GetEntityID())
        + ")/Child(" + std::to_string(child.GetEntityID()) + ") is not valid!");
      return;
    }

    mChildren[parent.GetRawEnttEntityID()].insert(child.GetRawEnttEntityID());
    mParent[child.GetRawEnttEntityID()] = parent.GetRawEnttEntityID();
  }

  void EntityManager::SetChildLayersToFollowParent(Entity const& parent) {
    if (!mRegistry.valid(parent.GetRawEnttEntityID())) {
      Debug::DebugLogger::GetInstance().LogError("[EntityManager] Parent(" + std::to_string(parent.GetEntityID()) + ") is not valid!");
      return;
    }

    if (!parent.HasComponent<Component::Layer>()) {
      Debug::DebugLogger::GetInstance().LogError("[EntityManager] Parent(" + parent.GetTag() + ") does not have a Layer!");
      return;
    }

    std::string const& parentLayer{ parent.GetComponent<Component::Layer>().name };
    if (mChildren.find(parent.GetRawEnttEntityID()) != mChildren.end()) {
      for (ECS::Entity child : mChildren[parent.GetRawEnttEntityID()]) {
        if (!child.HasComponent<Component::Layer>()) { continue; }

        Component::Layer& layer{ child.GetComponent<Component::Layer>() };

        if (layer.name != parentLayer) {
          layer.name = parentLayer;

          // updated prefab instances with overrides
          if (child.HasComponent<Component::PrefabOverrides>()) {
            child.GetComponent<Component::PrefabOverrides>().AddComponentOverride<Component::Layer>();
          }
        }

        // Recursively set the children of children
        if (mChildren.find(child.GetRawEnttEntityID()) != mChildren.end()) {
          SetChildLayersToFollowParent(child);
        }
      }
    }
  }

  void EntityManager::SetChildActiveToFollowParent(Entity const& parent) {
    if (!mRegistry.valid(parent.GetRawEnttEntityID())) {
      Debug::DebugLogger::GetInstance().LogError("[EntityManager] Parent(" + std::to_string(parent.GetEntityID()) + ") is not valid!");
      return;
    }

    if (!parent.HasComponent<Component::Tag>()) {
      Debug::DebugLogger::GetInstance().LogError("[EntityManager] Parent(" + std::to_string(parent.GetEntityID()) + ") does not have a Tag!");
      return;
    }
    
    bool const parentActive{ parent.IsActive() };
    if (mChildren.find(parent.GetRawEnttEntityID()) != mChildren.end()) {
      for (ECS::Entity child : mChildren[parent.GetRawEnttEntityID()]) {
        if (!child.HasComponent<Component::Tag>()) { continue; }

        Component::Tag& tag{ child.GetComponent<Component::Tag>() };
        if (tag.isActive != parentActive) {
          tag.isActive = parentActive;

          // updated prefab instances with overrides
          if (child.HasComponent<Component::PrefabOverrides>()) {
            child.GetComponent<Component::PrefabOverrides>().AddComponentOverride<Component::Tag>();
          }
        }

        // Recursively set the children of children
        if (mChildren.find(child.GetRawEnttEntityID()) != mChildren.end()) {
          SetChildActiveToFollowParent(child);
        }
      }
    }
  }

  bool EntityManager::RemoveParent(Entity const& child) {
    if (!mRegistry.valid(child.GetRawEnttEntityID())) {
      Debug::DebugLogger::GetInstance().LogError("[EntityManager] Entity " + std::to_string(child.GetEntityID()) + ") is not valid!");
      return false;
    }

    auto iter{ mParent.find(child.GetRawEnttEntityID()) };

    if (iter == mParent.end()) {
      Debug::DebugLogger::GetInstance().LogError("[EntityManager] Removing Non-existent Parent!");
      return false;
    }

    else {
      std::set<EntityID>& childList = mChildren[mParent[child.GetRawEnttEntityID()]];
      childList.erase(child.GetRawEnttEntityID());
      mParent.erase(child.GetRawEnttEntityID());
      return true;
    }
  }

  void EntityManager::RemoveEntity(Entity const& entity) {
    if (!mRegistry.valid(entity.GetRawEnttEntityID())) {
      Debug::DebugLogger::GetInstance().LogError("[EntityManager] Entity " + std::to_string(entity.GetEntityID()) + ") is not valid!");
      return;
    }

    auto iter{ mParent.find(entity.GetRawEnttEntityID()) };

    if (iter != mParent.end()) {
      // Entity has a parent, proceed to remove it from parent's child list
      std::set<EntityID> & childList = mChildren[mParent[entity.GetRawEnttEntityID()]];
      childList.erase(entity.GetRawEnttEntityID());
      mParent.erase(entity.GetRawEnttEntityID());
    }
    
    RecursivelyRemoveParentAndChild(entity.GetRawEnttEntityID());
  }

  void EntityManager::RecursivelyRemoveParentAndChild(EntityID entity) {
    std::set<EntityID> setOfChildren = mChildren[entity];
    for (EntityID child : setOfChildren) {
      IGE_EVENTMGR.DispatchImmediateEvent<Events::RemoveEntityEvent>(child);

      mParent.erase(child);
      RecursivelyRemoveParentAndChild(child);
    }
    mChildren.erase(entity);

    IGE_EVENTMGR.DispatchImmediateEvent<Events::RemoveEntityEvent>(entity);
    DeleteEntity(entity);
  }

  void EntityManager::Reset() {
    mRegistry.clear();
    mChildren.clear();
    mParent.clear();
  }

  entt::registry& EntityManager::GetRegistry() {
    return mRegistry;
  }

  void EntityManager::DeleteEntity(Entity entity) {
    if (!mRegistry.valid(entity.GetRawEnttEntityID())) {
      Debug::DebugLogger::GetInstance().LogError("[EntityManager] Entity " + std::to_string(entity.GetEntityID()) + ") is not valid!");
      return;
    }
    mRegistry.destroy(entity.GetRawEnttEntityID());
  }
} // namespace ECS