#pragma once
#include <Core/EntityManager.h>

namespace Systems {
  class System {
  public:
    System() : mEntityManager{ ECS::EntityManager::GetInstance() } {}
    virtual ~System() = default;

    virtual void Start() {}
    virtual void Update() {}
    virtual void Destroy() {}

  protected:
    ECS::EntityManager& mEntityManager; // may be bad, but i cant use ptr so
  };
} // namespace Systems
