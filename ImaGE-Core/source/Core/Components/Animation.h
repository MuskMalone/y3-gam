#pragma once
#include <glm/glm.hpp>
#include <set>
#include <vector>
#include <Asset/SmartPointer.h>

namespace Component {
  struct Animation {
    Animation() = default;

    void PlayAnimation(IGE::Assets::GUID guid, bool loop = false) noexcept {
      Reset();
      currentAnimation = guid;
      repeat = loop;
    }

    void Clear() noexcept {
      animations.clear();
      currentAnimation = {};
      nextKeyframe = 0;
      timeElapsed = 0.f;
      repeat = false;
    }

    void Reset() noexcept {
      nextKeyframe = 0;
      timeElapsed = 0.f;
    }

    std::set<IGE::Assets::GUID> animations;
    std::vector<unsigned> currentKeyframes;
    IGE::Assets::GUID currentAnimation;
    unsigned nextKeyframe;
    float timeElapsed;
    bool repeat;
  };
}
