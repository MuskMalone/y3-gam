#pragma once
#include "System.h"

namespace Systems {
  class TransformSystem : public System {
  public:
    TransformSystem(const char* name) : System(name) {}

    void Update() override;

  private:
    void UpdateTransform(ECS::Entity entity);
  };
} // namespace Systems