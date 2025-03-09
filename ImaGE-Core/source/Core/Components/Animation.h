#pragma once
#include <glm/glm.hpp>
#include <map>
#include <string>
#include <vector>
#include <Asset/SmartPointer.h>
#include <Animation/Keyframe.h>

namespace Component {
  struct Animation {
    using AnimationEntry = std::pair<std::string, IGE::Assets::GUID>;

    Animation() = default;

    // use this if the animation name doesn't matter
    // the animation does not have to be in the animation map
    void PlayAnimation(IGE::Assets::GUID guid, bool loop = false, std::string const& animName = {}) noexcept {
      Reset();
      currentAnimation = { animName, guid };
      repeat = loop;
    }
    void PlayAnimation(std::string const& name, bool loop = false) {
      Reset();
      currentAnimation = { name, animations[name] };
      repeat = loop;
    }

    void Pause() noexcept { paused = true; }
    void Resume() noexcept { paused = false; }
    bool IsPlayingAnimation() const noexcept { return currentAnimation.second; }

    // returns <name, guid>
    AnimationEntry const& GetCurrentAnimation() const noexcept{ return currentAnimation; }

    void SetCurrentAnimation(std::string name, IGE::Assets::GUID guid) {
      currentAnimation = std::make_pair(std::move(name), guid);
    }

    void RenameAnimation(std::string const& oldName, std::string const& newName) {
      auto iter{ animations.find(oldName) };
      if (iter == animations.end()) { return; }

      animations.emplace(newName, iter->second);
      animations.erase(iter);
    }


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
    AnimationEntry currentAnimation; // name, guid
    float timeElapsed;
    bool repeat, paused;
    bool relative;
  };
}
