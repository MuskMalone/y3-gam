#pragma once
#include <entt.hpp>
#include "Singleton.h"
//#include "Entity.h"

namespace ECS
{
  class Entity; // Forward Declaration

  class EntityManager : public Singleton <EntityManager>
  {
  public:
    friend class Entity;

    Entity CreateEntity();
    Entity CreateEntityWithTag(std::string tag);
    Entity CopyEntity(Entity entity);
    bool HasParent(Entity entity) const;
    bool HasChild(Entity entity) const;

    auto GetAllEntities();
    Entity GetEntityFromTag(std::string tag);
    Entity GetParentEntity(Entity const& child) const;
    std::set<Entity>& GetChildEntity(Entity const& parent);

    void SetParentEntity(Entity const& parent, Entity const& child);
    void SetChildEntity(Entity const& parent, Entity const& child);

    void RemoveParentEntity(Entity const& child);
    void RemoveChildEntity(Entity const& parent, Entity const& child);

    void Reset();
    void DeleteEntity(Entity entity);

    template<typename... Components>
    void RemoveComponentFromAllEntities();

    template<typename... Components>
    auto GetAllEntitiesWithComponents();

    template<typename... Components>
    void RemoveEntitiesWithComponents();

  private:
    entt::registry& GetRegistry();

  private:
    entt::registry m_registry;

    // entity is key, children are value
    std::map<Entity, std::set<Entity>> m_children;

    // entity is key, parent is value
    std::map<Entity, Entity> m_parent;
  };

  template<typename ...Components>
  inline void EntityManager::RemoveComponentFromAllEntities() {
    m_registry.clear<Components...>();
  }

  template<typename ...Components>
  inline auto EntityManager::GetAllEntitiesWithComponents() {
    return m_registry.view<Components...>();
  }

  template<typename ...Components>
  inline void EntityManager::RemoveEntitiesWithComponents() {
    auto view{ EntityManager::GetAllEntitiesWithComponents<Components...>() };
    m_registry.destroy(view.begin(), view.end());
  }

} // namespace ECS