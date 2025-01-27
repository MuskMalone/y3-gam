#include <pch.h>
#include "AnimationSystem.h"
#include <Core/Components/Animation.h>
#include <Core/Components/Transform.h>
#include <Core/Entity.h>
#include <FrameRateController/FrameRateController.h>

namespace {
  
}

namespace Systems {
  void AnimationSystem::Update() {
    float const deltaTime{ IGE_FRC.GetDeltaTime() };
    for (ECS::Entity entity : mEntityManager.GetAllEntitiesWithComponents<Component::Animation>()) {
      Component::Animation& animation{ entity.GetComponent<Component::Animation>() };

      if (animation.currentAnimation.empty() || !animation.animations.contains(animation.currentAnimation)) { continue; }

      std::vector<Component::Animation::Keyframe> const& keyframes{ animation.GetCurrentAnimation() };
      
      // assume keyframes are sorted by start time followed by end time
      // add any more keyframes in range into the current list
      for (unsigned i{ animation.lastKeyframeProcessed }; i < keyframes.size() && animation.timeElapsed >= keyframes[i].startTime; ++i) {
        animation.currentKeyframes.emplace_back(i);
      }

      Component::Transform& trans{ entity.GetComponent<Component::Transform>() };
      // get the current t value mapped to [0, 1]
      float const normalizedTime{ (animation.timeElapsed - 
        keyframes[animation.lastKeyframeProcessed].startTime) / keyframes[animation.lastKeyframeProcessed].duration};
      float const toBeDeltaTime{ animation.timeElapsed + deltaTime };

      // execute all keyframes in range by setting the interpolated value based on the current t
      for (auto iter{ animation.currentKeyframes.cbegin() }; iter != animation.currentKeyframes.cend();) {
        Component::Animation::Keyframe const& keyframe{ keyframes[*iter] };

        // if the keyframe ends this frame, complete and remove it
        bool const lastFrame{ toBeDeltaTime >= keyframe.GetEndTime() };

        switch (keyframe.type) {
        case Component::Animation::TRANSLATION:
          if (lastFrame) {
            trans.position = std::get<glm::vec3>(keyframe.endValue);
          }
          else {
            trans.position = glm::mix(std::get<glm::vec3>(keyframe.startValue), std::get<glm::vec3>(keyframe.endValue), normalizedTime);
          }
          trans.modified = true;
          break;
        case Component::Animation::ROTATION:
          if (lastFrame) {
            trans.SetLocalRotWithEuler(std::get<glm::vec3>(keyframe.endValue));
          }
          else {
            trans.SetLocalRotWithEuler(
              glm::mix(std::get<glm::vec3>(keyframe.startValue), std::get<glm::vec3>(keyframe.endValue), normalizedTime)
            );
          }
          trans.modified = true;
          break;
        case Component::Animation::SCALE:
          if (lastFrame) {
            trans.scale = std::get<glm::vec3>(keyframe.endValue);
          }
          else {
            trans.scale = glm::mix(std::get<glm::vec3>(keyframe.startValue), std::get<glm::vec3>(keyframe.endValue), normalizedTime);
          }
          trans.modified = true;
          break;
        case Component::Animation::NONE:
          break;
        default:
          IGE_DBGLOGGER.LogError("[AnimationSystem] Invalid Animation Keyframe type: " + std::to_string(keyframe.type));
          break;
        }

        if (lastFrame) {
          iter = animation.currentKeyframes.erase(iter);
        }
        else {
          ++iter;
        }
      }

      animation.timeElapsed = toBeDeltaTime; // update elapsed time

      // if reached the end, reset if repeat flag is set
      if (animation.currentKeyframes.empty()) {
        animation.Reset();

        if (!animation.repeat) {
          animation.currentAnimation.clear();
        }
      }
    }
  }

} // namespace Systems
