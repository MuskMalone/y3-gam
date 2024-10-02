#pragma once
#include <unordered_map>
#include <memory>
#include "System.h"

namespace Systems {
  class SystemManager
  {
  public:

    void UpdateSystems();

  private:

    std::unordered_map<const char*, std::shared_ptr<System>> mSystems;
  };

} // namespace Systems
