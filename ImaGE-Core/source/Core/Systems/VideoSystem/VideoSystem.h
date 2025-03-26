#pragma once
#include "../System.h"

namespace Systems {
  class VideoSystem : public System
  {
  public:
    VideoSystem(const char* name);

    void Update() override;
    void PausedUpdate() override {}  // do nothing in paused update
  };
} // namespace Systems