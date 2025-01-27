#include <pch.h>
#include "AnimationSystem.h"
#include <Core/Entity.h>
#include <Core/Components/Animation.h>
#include <Core/Components/Transform.h>
#include <Asset/IGEAssets.h>
#include <Animation/AnimationData.h>

#include <FrameRateController/FrameRateController.h>

namespace Systems {
  void AnimationSystem::Update() {
    float const deltaTime{ IGE_FRC.GetDeltaTime() };
    IGE::Assets::AssetManager& am{ IGE_ASSETMGR };

    for (ECS::Entity entity : mEntityManager.GetAllEntitiesWithComponents<Component::Animation>()) {
      Component::Animation& animation{ entity.GetComponent<Component::Animation>() };

      if (!animation.currentAnimation || !animation.animations.contains(animation.currentAnimation)) { continue; }

      try {
        am.LoadRef<IGE::Assets::AnimationAsset>(animation.currentAnimation);
      }
      catch (Debug::ExceptionBase&) {
        IGE_DBGLOGGER.LogError("[AnimationSystem] Unable to get animation " +
          std::to_string(static_cast<uint64_t>(animation.currentAnimation)) + " of Entity " + entity.GetTag());
        continue;
      }

      auto const&[name, keyframes] { am.GetAsset<IGE::Assets::AnimationAsset>(animation.currentAnimation)->mAnimData };

      // assume keyframes are sorted by start time followed by end time
      // add any more keyframes in range into the current list
      for (unsigned& i{ animation.nextKeyframe }; i < keyframes.size() && animation.timeElapsed >= keyframes[i].startTime; ++i) {
        animation.currentKeyframes.emplace_back(i);
      }

      Component::Transform& trans{ entity.GetComponent<Component::Transform>() };
      // get the current t value mapped to [0, 1]
      float const normalizedTime{ (animation.timeElapsed -
        keyframes[animation.nextKeyframe].startTime) / keyframes[animation.nextKeyframe].duration };
      float const toBeDeltaTime{ animation.timeElapsed + deltaTime };

      // execute all keyframes in range by setting the interpolated value based on the current t
      for (auto iter{ animation.currentKeyframes.cbegin() }; iter != animation.currentKeyframes.cend();) {
        Anim::Keyframe const& keyframe{ keyframes[*iter] };

        // if the keyframe ends this frame, complete and remove it
        bool const lastFrame{ toBeDeltaTime >= keyframe.GetEndTime() };

        switch (keyframe.type) {
        case Anim::KeyframeType::TRANSLATION:
          if (lastFrame) {
            trans.position = std::get<glm::vec3>(keyframe.endValue);
          }
          else {
            trans.position = glm::mix(std::get<glm::vec3>(keyframe.startValue), std::get<glm::vec3>(keyframe.endValue), normalizedTime);
          }
          trans.modified = true;
          break;
        case Anim::KeyframeType::ROTATION:
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
        case Anim::KeyframeType::SCALE:
          if (lastFrame) {
            trans.scale = std::get<glm::vec3>(keyframe.endValue);
          }
          else {
            trans.scale = glm::mix(std::get<glm::vec3>(keyframe.startValue), std::get<glm::vec3>(keyframe.endValue), normalizedTime);
          }
          trans.modified = true;
          break;
        case Anim::KeyframeType::NONE:
          break;
        default:
          IGE_DBGLOGGER.LogError("[AnimationSystem] Invalid Animation Keyframe type: " + std::to_string(static_cast<int>(keyframe.type)));
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
          animation.currentAnimation = {};
        }
      }
    }
  }

} // namespace Systems
