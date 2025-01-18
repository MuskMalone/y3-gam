#pragma once
#include <vector>
#include <string>
#include <Core/Entity.h>

namespace Scenes {
  struct Scene {
    Scene(std::string name);

    std::string mName;
    std::vector<ECS::Entity> mEntities;
  };
} // namespace Scenes
