#pragma once
#include <Core/Systems/System.h>
#include <Events/EventCallback.h>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtx/easing.hpp>
#include <Animation/Keyframe.h>

namespace Systems {
  class AnimationSystem : public System {
  public:
    AnimationSystem(const char* name);

    void Update() override;
    void PausedUpdate() override;

  private:
    // start, end, normalizedTime
    template <typename T>
    using InterpFunc = std::function<T(Anim::Keyframe::ValueType const&, Anim::Keyframe::ValueType const&, float)>;

    template <typename T>
    T LinearInterpolation(Anim::Keyframe::ValueType const& start, Anim::Keyframe::ValueType const& end, float normalizedTime) {
      return glm::mix(std::get<T>(start), std::get<T>(end), normalizedTime);
    }

    template <typename T>
    T EaseIn(Anim::Keyframe::ValueType const& start, Anim::Keyframe::ValueType const& end, float normalizedTime) {
      T const& startRaw{ std::get<T>(start) };
      return (std::get<T>(end) - startRaw) * glm::quadraticEaseIn(normalizedTime) + startRaw;
    }

    template <typename T>
    T EaseOut(Anim::Keyframe::ValueType const& start, Anim::Keyframe::ValueType const& end, float normalizedTime) {
      T const& startRaw{ std::get<T>(start) };
      return (std::get<T>(end) - startRaw) * glm::quadraticEaseOut(normalizedTime) + startRaw;
    }

    template <typename T>
    T EaseInOut(Anim::Keyframe::ValueType const& start, Anim::Keyframe::ValueType const& end, float normalizedTime) {
      T const& startRaw{ std::get<T>(start) };
      return (std::get<T>(end) - startRaw) * glm::quadraticEaseInOut(normalizedTime) + startRaw;
    }

    EVENT_CALLBACK_DECL(OnEntityPreview);

    InterpFunc<glm::vec3> interpFuncs[static_cast<int>(Anim::InterpolationType::NUM_TYPES)];
  };
}
