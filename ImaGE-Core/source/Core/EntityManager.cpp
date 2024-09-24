#include <pch.h>
#include "EntityManager.h"
#include "Entity.h"

namespace ECS {
  Entity EntityManager::CreateEntity() {
    Entity entity(mRegistry.create());

    // Default all created entities to have the following components:
    entity.EmplaceComponent<Component::Tag>();
    entity.EmplaceComponent<Component::Transform>();

    return entity;
  }
  
  /*!*********************************************************************
  \brief
    Allows an entity to be created with a given ID and name. This
    function can only be invoked by classes the "Exclusive" class
    explicitly friends. Only friend classes can create an instance of
    Exclusive by specifying "{}" in the arg list.
  \param entityID
    The ID of the entity
  \return
    The created entity
  ************************************************************************/
  Entity EntityManager::CreateEntityWithID([[maybe_unused]] ECSKey key, EntityID entityID) {
      return Entity(mRegistry.create(entityID));
  }

  Entity EntityManager::CreateEntityWithTag(std::string const& tag) {
    Entity entity(mRegistry.create());
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
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Entity is not valid!" << std::endl;
      return false;
    }

    auto iter{ mParent.find(entity.GetRawEnttEntityID()) };
    return iter != mParent.end();
  }

  bool EntityManager::HasChild(Entity entity) const {
    if (!mRegistry.valid(entity.GetRawEnttEntityID())) {
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Entity is not valid!" << std::endl;
      return Entity();
    }

    auto iter{ mChildren.find(entity.GetRawEnttEntityID()) };
    // added 2nd check to ensure there are no children
    // also removes the need to erase from map when deleting
    return iter != mChildren.end() && !iter->second.empty();
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
    if (!mRegistry.valid(child.GetRawEnttEntityID())) {
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Child is not valid!" << std::endl;
      return Entity();
    }

    auto iter{ mParent.find(child.GetRawEnttEntityID()) };

    if (iter == mParent.end()) {
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Entity: " << child.GetTag() << " does not have a Parent!\n";
    }

    return iter->second;
  }

  std::vector<Entity> EntityManager::GetChildEntity(Entity const& parent) {
    if (!mRegistry.valid(parent.GetRawEnttEntityID())) {
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Parent is not valid!" << std::endl;
      return std::vector<Entity>();
    }

    auto iter{ mChildren.find(parent.GetRawEnttEntityID()) };

    if (iter == mChildren.end()) {
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

  std::unordered_map<EntityManager::EntityID, std::set<EntityManager::EntityID>> const& EntityManager::GetChildrenMap() const {
      return mChildren;
  }

  std::unordered_map<EntityManager::EntityID, EntityManager::EntityID> const& EntityManager::GetParentMap() const {
      return mParent;
  }

  void EntityManager::SetParentEntity(Entity const& parent, Entity const& child) {
    if (!mRegistry.valid(parent.GetRawEnttEntityID()) || 
      !mRegistry.valid(child.GetRawEnttEntityID())) {
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Parent/Child is not valid!" << std::endl;
      return;
    }

    mParent[child.GetRawEnttEntityID()] = parent.GetRawEnttEntityID();
    mChildren[parent.GetRawEnttEntityID()].insert(child.GetRawEnttEntityID());
  }

  void EntityManager::SetChildEntity(Entity const& parent, Entity const& child) {
    if (!mRegistry.valid(parent.GetRawEnttEntityID()) || 
      !mRegistry.valid(child.GetRawEnttEntityID())) {
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Parent/Child is not valid!" << std::endl;
      return;
    }

    mChildren[parent.GetRawEnttEntityID()].insert(child.GetRawEnttEntityID());
    mParent[child.GetRawEnttEntityID()] = parent.GetRawEnttEntityID();
  }

  bool EntityManager::RemoveParent(Entity const& child) {
    if (!mRegistry.valid(child.GetRawEnttEntityID())) {
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Entity is not valid!" << std::endl;
      return false;
    }

    auto iter{ mParent.find(child.GetRawEnttEntityID()) };

    if (iter == mParent.end()) {
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Removing Non-existent Parent!\n";
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
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Entity is not valid!" << std::endl;
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
      mParent.erase(child);
      RecursivelyRemoveParentAndChild(child);
    }
    mChildren.erase(entity);
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
      // @TODO: REPLACE WITH LOGGING SYSTEM
      std::cout << "Entity is not valid!" << std::endl;
      return;
    }
    mRegistry.destroy(entity.GetRawEnttEntityID());
  }
} // namespace ECS