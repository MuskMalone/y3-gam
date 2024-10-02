template <typename T>
void SystemManager::RegisterSystem(const char* name) {
  mSystems.emplace(typeid(T).name(), std::make_shared<T>(name));
}

template <typename T>
std::shared_ptr<T> SystemManager::GetSystem() const {
  const char* sysName{ typeid(T).name() };
  if (!mSystems.contains(sysName)) {
    throw Debug::Exception<SystemManager>(Debug::LVL_ERROR, std::string("Trying to get unregistered system of type ") + typeid(T).name());
  }

  return std::static_pointer_cast<T>(mSystems.at(sysName));
}
