#pragma once
#include <cstdint>

namespace Scenes
{

  enum SceneState : uint8_t
  {
    PLAYING = 1,
    PAUSED = 2,
    STOPPED = 4,
    PREFAB_EDITOR = 8
  };

} // namespace Scenes
