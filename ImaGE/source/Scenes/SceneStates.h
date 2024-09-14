#pragma once
#include <cstdint>

namespace Scenes
{

  enum class SceneState : uint8_t
  {
    PLAYING,
    PAUSED,
    STOPPED,
    PREFAB_EDITOR
  };

} // namespace Scenes
