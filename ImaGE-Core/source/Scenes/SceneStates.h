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
    NO_SCENE = 1,
    PLAYING = 2,
    PAUSED = 4,
    STOPPED = 8,
    PREFAB_EDITOR = 16
  };

} // namespace Scenes
