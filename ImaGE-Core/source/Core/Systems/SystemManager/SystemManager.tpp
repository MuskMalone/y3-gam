#include <DebugTools/DebugLogger/DebugLogger.h>

template <typename T>
void SystemManager::RegisterSystem(const char* name) {
  static_assert(!std::is_same<decltype(&System::Update), decltype(&T::Update)>::value,
    "System must override System::Update!");

  SystemPtr sys{ std::make_shared<T>(name) };
  mNameToSystem.emplace(typeid(T).name(), sys);
  mSystems.emplace_back(std::move(sys));
}

template <typename T>
std::weak_ptr<T> SystemManager::GetSystem() const {
  const char* sysName{ typeid(T).name() };
  if (!mNameToSystem.contains(sysName)) {
    throw Debug::Exception<SystemManager>(Debug::LVL_ERROR, Msg(std::string("Trying to get unregistered system of type ") + typeid(T).name()));
  }

  return std::static_pointer_cast<T>(mNameToSystem.at(sysName));
}

template <typename... Systems>
void SystemManager::UpdateSelectedSystems() {
  UpdateSystems({ typeid(Systems).name()... });

  //LateUpdateSystems({ typeid(Systems).name()... });
}
