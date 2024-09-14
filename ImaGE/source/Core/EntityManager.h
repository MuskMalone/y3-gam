#pragma once
#include <entt.hpp>
#include "Component/Components.h"
#include "Singleton.h"

namespace ECS {
  class Entity; // Forward Declaration

  class EntityManager : public Singleton <EntityManager> {
  public:
    using EntityID = entt::entity;
    friend class Entity;

    Entity CreateEntity();
    Entity CreateEntityWithTag(std::string const& tag);
    Entity CopyEntity(Entity entity);
    bool HasParent(Entity entity) const;
    bool HasChild(Entity entity) const;

    auto GetAllEntities();
    Entity GetEntityFromTag(std::string tag);
    Entity GetParentEntity(Entity const& child) const;
    std::vector<Entity> GetChildEntity(Entity const& parent);

    void SetParentEntity(Entity const& parent, Entity const& child);
    void SetChildEntity(Entity const& parent, Entity const& child);

    std::unordered_map<EntityID, std::set<EntityID>> const& GetChildrenMap() const;
    std::unordered_map<EntityID, EntityID> const& GetParentMap() const;

    bool RemoveParent(Entity const& child);
    void RemoveEntity(Entity const& entity);
    void Reset();
    
    template<typename... Components>
    void RemoveComponentFromAllEntities();

    template<typename... Components>
    auto GetAllEntitiesWithComponents();

    template<typename... Components>
    void RemoveEntitiesWithComponents();

  private:
    void RecursivelyRemoveParentAndChild(EntityID entityID);
    entt::registry& GetRegistry();
    void DeleteEntity(Entity entity);

  private:
    entt::registry mRegistry;

    // entity is key, children are value
    std::unordered_map<EntityID, std::set<EntityID>> mChildren;

    // entity is key, parent is value
    std::unordered_map<EntityID, EntityID> mParent;
  };

  inline auto EntityManager::GetAllEntities() {
    return mRegistry.view<Component::Tag>();
  }

  template<typename ...Components>
  inline void EntityManager::RemoveComponentFromAllEntities() {
    mRegistry.clear<Components...>();
  }

  template<typename ...Components>
  inline auto EntityManager::GetAllEntitiesWithComponents() {
    return mRegistry.view<Components...>();
  }

  template<typename ...Components>
  inline void EntityManager::RemoveEntitiesWithComponents() {
    auto view{ EntityManager::GetAllEntitiesWithComponents<Components...>() };
    mRegistry.destroy(view.begin(), view.end());
  }
} // namespace ECS