#pragma once
#include <entt/entt.hpp>
#include "EntityManager.h"
#include <rttr/type.h>

namespace ECS {
  class Entity {
  public:
    using EntityID = entt::entity;

  public:
    Entity() = default;
    Entity(EntityID entityID);

    uint32_t GetEntityID() const;
    EntityID GetRawEnttEntityID() const;
    std::string const& GetTag() const;
    void SetTag(std::string const& tag);
    bool IsActive() const;
    void SetIsActive(bool isActiveFlag);

    operator bool() const;
    bool operator==(const Entity& entity) const;
    bool operator!=(const Entity& entity) const;

    template<typename T, typename... Args>
    T& EmplaceComponent(Args&&... args);

    template<typename T, typename... Args>
    T& EmplaceOrReplaceComponent(Args&&... args);

    template<typename T>
    T& GetComponent();

    template<typename T>
    T const& GetComponent() const;

    template<typename... Components>
    void RemoveComponent();

    template<typename... Components>
    bool HasComponent() const;

  private:
    void DispatchRemoveComponentEvent(std::initializer_list<rttr::type> types);

    EntityID mId{ entt::null };
  };

  // Use this function for new components (you know that the entity does not have the component)
  template<typename T, typename ...Args>
  inline T& Entity::EmplaceComponent(Args && ...args) {
    return EntityManager::GetInstance().GetRegistry().emplace<T>(mId, std::forward<Args>(args)...);
  }

  // Use this function when it is unknown whether the entity already owns the component
  template<typename T, typename ...Args>
  inline T& Entity::EmplaceOrReplaceComponent(Args && ...args) {
    return EntityManager::GetInstance().GetRegistry().emplace_or_replace<T>(mId, std::forward<Args>(args)...);
  }

  template<typename T>
  inline T& Entity::GetComponent() {
    return EntityManager::GetInstance().GetRegistry().get<T>(mId);
  }

  template<typename T>
  inline T const& Entity::GetComponent() const {
    const auto& cRegistry{ EntityManager::GetInstance().GetRegistry() };
    return cRegistry.get<T>(mId);
  }

  template<typename... Components>
  inline void Entity::RemoveComponent() {
    DispatchRemoveComponentEvent({ rttr::type::get<Components>()... });

    EntityManager::GetInstance().GetRegistry().remove<Components...>(mId);
  }

  template<typename... Components>
  inline bool Entity::HasComponent() const {
    return EntityManager::GetInstance().GetRegistry().all_of<Components...>(mId);
  }
} // namespace ECS