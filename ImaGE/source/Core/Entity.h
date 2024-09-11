#pragma once
#include <entt.hpp>

class Entity {
  using EntityID = entt::entity;

public:
  Entity() = default;
  Entity(EntityID entID);
  uint32_t GetEntityID(Entity entity);

  template<typename T, typename... Args>
  T& AddComponent(Args&&... args);

  template<typename T, typename... Args>
  T& ReplaceComponent(Args&&... args);

  template<typename T>
  T& GetComponent();

  template<typename T>
  void RemoveComponent();

  template<typename T>
  bool HasComponent();

private:
  EntityID m_id;
};

template<typename T, typename ...Args>
inline T& Entity::AddComponent(Args && ...args) {
  // TODO: insert return statement here
}

template<typename T, typename ...Args>
inline T& Entity::ReplaceComponent(Args && ...args) {
  // TODO: insert return statement here
}

template<typename T>
inline T& Entity::GetComponent() {
  // TODO: insert return statement here
}

template<typename T>
inline void Entity::RemoveComponent() {

}

template<typename T>
inline bool Entity::HasComponent() {
  return false;
}
