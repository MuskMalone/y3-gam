#pragma once
#include <vector>
#include <Core/Entity.h>

namespace Scenes {
  class Scene {
  public:
    Scene();

  private:
    std::vector<ECS::Entity> mEntities;
  };
} // namespace Scenes
