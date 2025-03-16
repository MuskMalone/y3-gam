#pragma once
#include <variant>
#include <memory>

namespace Anim {
  struct Keyframe;
  using Node = std::shared_ptr<Keyframe>;
  using NodeId = int;

  enum class KeyframeType {
    NONE = 0,
    TRANSLATION,
    SCALE,
    ROTATION,
    NUM_TYPES
  };

  enum class InterpolationType {
    LINEAR = 0,
    EASE_IN,
    EASE_OUT,
    EASE_INOUT,
    NUM_TYPES
  };

  struct RootKeyframe {
    RootKeyframe() : nextNodes{}, startPos{}, startRot{}, startScale{ 1.f, 1.f, 1.f } {}

    std::vector<Node> nextNodes;
    glm::vec3 startPos, startRot, startScale;
  };

  struct Keyframe {
    using ValueType = std::variant<glm::vec3>; // may change to rttr::variant if cant serialize

    Keyframe() : nextNodes{}, startValue{}, endValue{},
      type{ KeyframeType::NONE }, interpolationType{}, startTime {}, duration{}, id{} {}
    Keyframe(ValueType endVal, KeyframeType _type, InterpolationType interpolation, float _startTime, float _duration) :
      nextNodes{}, startValue{}, endValue{ std::move(endVal) },
      type{ _type }, interpolationType{ interpolation },
      startTime{ _startTime }, duration{ _duration }, id{} {

    }

    template <typename T>
    T GetNormalizedValue() const { return std::get<T>(endValue) - std::get<T>(startValue); }

    inline float GetEndTime() const noexcept { return startTime + duration; }

    std::vector<Node> nextNodes;
    ValueType startValue, endValue;
    KeyframeType type;
    InterpolationType interpolationType;
    float startTime, duration;
    NodeId id;  // only for editor but have to put it here for now welp
  };
}
