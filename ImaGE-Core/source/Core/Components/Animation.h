#pragma once
#include <glm/glm.hpp>
#include <map>
#include <string>
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
    void PlayAnimation(std::string const& name, bool loop = false) {
      PlayAnimation(animations[name], loop);
    }

    void RenameAnimation(std::string const& oldName, std::string const& newName) {
      auto iter{ animations.find(oldName) };
      if (iter == animations.end()) { return; }

      animations.emplace(newName, iter->second);
      animations.erase(iter);
    }

    void Pause() noexcept { paused = true; }
    void Resume() noexcept { paused = false; }

    void Reset() noexcept {
      timeElapsed = 0.f;
      currentKeyframes.clear();
      paused = false;
    }

    void Clear() noexcept {
      animations.clear();
      currentKeyframes.clear();
      currentAnimation = {};
      timeElapsed = 0.f;
      repeat = paused = false;
    }

    std::map<std::string, IGE::Assets::GUID> animations;
    std::vector<Anim::Node> currentKeyframes;
    IGE::Assets::GUID currentAnimation;
    float timeElapsed;
    bool repeat, paused;
  };
}
