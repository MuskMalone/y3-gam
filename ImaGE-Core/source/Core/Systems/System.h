#pragma once
#include <Core/EntityManager.h>
#include <memory>
#include <string>

namespace Systems {
  class System {
  public:
    System(std::string name) : mEntityManager{ ECS::EntityManager::GetInstance() }, mName{ std::move(name) } {}
    virtual ~System() = default;

    virtual void Start() {}
    virtual void Update() {}
    virtual void LateUpdate() {}
    virtual void Destroy() {}
    
    std::string const& GetName() const noexcept { return mName; }

  protected:
    ECS::EntityManager& mEntityManager; // may be bad, but i cant use ptr so

  private:
    std::string const mName;
  };
} // namespace Systems
