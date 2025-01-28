#pragma once
#include "Keyframe.h"
#include <string>
#include <vector>

namespace Anim {
  struct AnimationData {
    AnimationData() = default;
    AnimationData(std::string animName) : name{ std::move(animName) }, keyframes{} {}

    std::string name;
    std::vector<Keyframe> keyframes;
  };
}
