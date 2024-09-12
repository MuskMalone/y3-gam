#pragma once
#include <cstdint>

namespace Scenes
{

  enum class SceneState : uint8_t
  {
    STOPPED,
    PLAYING,
    PAUSED
  };

} // namespace Scenes
