#pragma once
#include <entt.hpp>
#include "Component.h"
#include "Singleton.h"

class Entity; // Forward Declaration

class EntityManager : public Singleton <EntityManager> {
public:
  Entity CreateEntity();
  Entity CreateEntityWithTag(std::string tag);
  void RemoveEntity(Entity entity);
  Entity GetEntityFromTag(std::string tag);
  void Reset();
  entt::registry& GetRegistry();

  template<typename... Components>
  void RemoveComponentFromAllEntities();

  template<typename... Components>
  auto GetAllEntitiesWithComponents();

  template<typename... Components>
  void RemoveEntitiesWithComponents();

private:
  entt::registry m_registry;
};

template<typename... Components>
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
