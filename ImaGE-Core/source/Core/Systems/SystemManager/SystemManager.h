#pragma once
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <Core/Systems/System.h>

namespace Systems {
  class SystemManager
  {
  public:
    template <typename T>
    void RegisterSystem(const char* name);
    
    template <typename T>
    std::shared_ptr<T> GetSystem() const;

    void InitSystems();
    void UpdateSystems();
    void Shutdown();

  private:
    using SystemPtr = std::shared_ptr<System>;

    std::unordered_map<const char*, SystemPtr> mSystems;
  };
#include "SystemManager.tpp"
} // namespace Systems
