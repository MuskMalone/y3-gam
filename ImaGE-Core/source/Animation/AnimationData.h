#pragma once
#include "Keyframe.h"
#include <string>
#include <vector>

namespace Anim {
  struct AnimationData {
    AnimationData() = default;

    RootKeyframe rootKeyframe;
  };
}
