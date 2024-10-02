#pragma once
#include "System.h"

namespace Systems {
  class TransformSystem : System {
  public:
    TransformSystem() : System() {}

    void Update() override;

  private:
    void UpdateTransform(ECS::Entity entity);
  };
} // namespace Systems