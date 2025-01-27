#pragma once
#include <variant>
#include <memory>

namespace Anim {
  enum class KeyframeType {
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
}
