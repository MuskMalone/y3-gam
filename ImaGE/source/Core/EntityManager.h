#pragma once
#include <entt.hpp>

#include "Singleton.h"
#include "Entity.h"

class EntityManager : public Singleton <EntityManager> {
public:
  Entity CreateEntity();
  Entity CreateEntityWithTag(std::string tag);
  void RemoveEntity(Entity entity);
  Entity GetEntityFromTag(std::string tag);
  void Reset();

  template<typename... Args>
  void RemoveComponentFromAllEntities();

private:
  entt::registry m_registry;
};

template<typename... Args>
inline void EntityManager::RemoveComponentFromAllEntities() {
  (m_registry.clear<Args>(), ...);
}