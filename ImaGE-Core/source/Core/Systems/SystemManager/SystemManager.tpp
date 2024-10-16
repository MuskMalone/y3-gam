#include <DebugTools/DebugLogger/DebugLogger.h>

template <typename T>
void SystemManager::RegisterSystem(const char* name) {
  SystemPtr sys{ std::make_shared<T>(name) };
  mNameToSystem.emplace(typeid(T).name(), sys);
  mSystems.emplace_back(std::move(sys));
}

template <typename T>
std::weak_ptr<T> SystemManager::GetSystem() const {
  const char* sysName{ typeid(T).name() };
  if (!mNameToSystem.contains(sysName)) {
    throw std::logic_error(std::string("Trying to get unregistered system of type ") + typeid(T).name());
  }

  return std::static_pointer_cast<T>(mNameToSystem.at(sysName));
}
