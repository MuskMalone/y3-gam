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
  T const& GetComponent() const;

  template<typename T>
  T& TryGetComponent();

  template<typename T>
  T const& TryGetComponent() const;

  template<typename... Components>
  void RemoveComponent();

  template<typename... Components>
  bool HasComponent();

private:
  EntityID m_id{ entt::null };
};

// Use this function for new components (you know that the entity does not have the component)
template<typename T, typename ...Args>
inline T& Entity::EmplaceComponent(Args && ...args) {
  return EntityManager::GetInstance().GetRegistry().emplace<T>(m_id, std::forward<Args>(args)...);
}

// Use this function when it is unknown whether the entity already owns the component
template<typename T, typename ...Args>
inline T& Entity::EmplaceOrReplaceComponent(Args && ...args) {
  return EntityManager::GetInstance().GetRegistry().emplace_or_replace<T>(m_id, std::forward<Args>(args)...);
}

template<typename T>
inline T& Entity::GetComponent() {
  return EntityManager::GetInstance().GetRegistry().get<T>(m_id);
}

template<typename T>
inline T const& Entity::GetComponent() const {
  const auto& cRegistry{ EntityManager::GetInstance().GetRegistry() };
  return cRegistry.get<T>(m_id);
}

// Use this over GetComponent() if you are unsure if the entity has the component
template<typename T>
inline T& Entity::TryGetComponent() {
  return EntityManager::GetInstance().GetRegistry().try_get<T>(m_id);
}

// Use this over GetComponent() if you are unsure if the entity has the component
template<typename T>
inline T const& Entity::TryGetComponent() const {
  const auto& cRegistry{ EntityManager::GetInstance().GetRegistry() };
  return cRegistry.try_get<T>(m_id);
}

template<typename... Components>
inline void Entity::RemoveComponent() {
  EntityManager::GetInstance().GetRegistry().remove<Components...>(m_id);
}

template<typename... Components>
inline bool Entity::HasComponent() {
  
  return false;
}
