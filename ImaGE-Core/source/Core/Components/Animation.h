#pragma once
#include <glm/glm.hpp>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace Component {
  struct Animation {
    Animation() = default;

    enum KeyframeType {
      NONE = 0,
      TRANSLATION,
      SCALE,
      ROTATION,
      NUM_TYPES
    };

    struct Keyframe {
      using ValueType = std::variant<glm::vec3>; // may change to rttr::variant if cant serialize

      Keyframe() : startValue{}, endValue{}, type{ KeyframeType::NONE }, startTime{}, duration{} {}
      Keyframe(ValueType startVal, ValueType endVal, KeyframeType _type, float _startTime, float _duration) :
        startValue{ std::move(startVal) }, endValue{ std::move(endVal) }, type{ _type }, startTime{ _startTime }, duration{ _duration } {
        
      }
      
      template <typename T>
      T GetNormalizedValue() const { return std::get<T>(endValue) - std::get<T>(startValue); }

      inline float GetEndTime() const noexcept { return startTime + duration; }

      ValueType startValue, endValue;
      KeyframeType type;
      float startTime, duration;
    };

    void PlayAnimation(std::string const& animName, bool loop = false) noexcept {
      Reset();
      currentAnimation = animName;
      repeat = loop;
    }
    std::vector<Keyframe> const& GetCurrentAnimation() const { return animations.at(currentAnimation); }
    std::vector<Keyframe> const& GetAnimation(std::string const& key) const { return animations.at(key); }

    void Clear() noexcept {
      animations.clear();
      currentAnimation.clear();
      lastKeyframeProcessed = 0;
      timeElapsed = 0.f;
      repeat = false;
    }

    void Reset() noexcept {
      lastKeyframeProcessed = 0;
      timeElapsed = 0.f;
    }

    std::map<std::string, std::vector<Keyframe>> animations;
    std::vector<unsigned> currentKeyframes;
    std::string currentAnimation;
    unsigned lastKeyframeProcessed;
    float timeElapsed;
    bool repeat;
  };
}
