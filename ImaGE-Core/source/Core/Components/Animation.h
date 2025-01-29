#pragma once
#include <glm/glm.hpp>
#include <set>
#include <vector>
#include <Asset/SmartPointer.h>
#include <Animation/Keyframe.h>

namespace Component {
  struct Animation {
    Animation() = default;

    void PlayAnimation(IGE::Assets::GUID guid, bool loop = false) noexcept {
      Reset();
      currentAnimation = guid;
      repeat = loop;
    }

    void Reset() noexcept {
      timeElapsed = 0.f;
      currentKeyframes.clear();
    }

    void Clear() noexcept {
      animations.clear();
      currentKeyframes.clear();
      currentAnimation = {};
      timeElapsed = 0.f;
      repeat = false;
    }

    std::set<IGE::Assets::GUID> animations;
    std::vector<Anim::Node> currentKeyframes;
    IGE::Assets::GUID currentAnimation;
    float timeElapsed;
    bool repeat;
  };
}
