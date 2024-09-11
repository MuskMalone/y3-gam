#pragma once
#include <entt.hpp>
#include "EntityManager.h"

class Entity {
public:
  using EntityID = entt::entity;

public:
  Entity() = default;
  Entity(EntityID entID);
  uint32_t GetEntityID() const;
  EntityID GetRawEnttEntityID() const;
  std::string GetTag() const;

  template<typename T, typename... Args>
  T& EmplaceComponent(Args&&... args);

  template<typename T, typename... Args>
  T& EmplaceOrReplaceComponent(Args&&... args);

  template<typename T>
  T& GetComponent();

  template<typename T>
  void RemoveComponent();

  template<typename T>
  bool HasComponent();

private:
  EntityID m_id{ entt::null };
};

// Use this function for new components
template<typename T, typename ...Args>
inline T& Entity::EmplaceComponent(Args && ...args) {
  return EntityManager::GetInstance().GetRegistry().emplace<T>(m_id, std::forward<Args>(args)...);
}

template<typename T, typename ...Args>
inline T& Entity::EmplaceOrReplaceComponent(Args && ...args) {
  
}

template<typename T>
inline T& Entity::GetComponent() {
  
}

template<typename T>
inline void Entity::RemoveComponent() {

}

template<typename T>
inline bool Entity::HasComponent() {
  return false;
}
