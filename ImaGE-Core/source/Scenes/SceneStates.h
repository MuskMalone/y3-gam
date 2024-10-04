/*!*********************************************************************
\file   SceneStates.h
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Definition of enum SceneState, which describes the current
        state of the scene.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
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
