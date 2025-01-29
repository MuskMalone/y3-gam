#include <pch.h>
#include "AnimationSystem.h"
#include <Core/Entity.h>
#include <Core/Components/Animation.h>
#include <Core/Components/Transform.h>
#include <Asset/IGEAssets.h>
#include <Animation/AnimationData.h>

#include <FrameRateController/FrameRateController.h>

namespace {
  void InitTransform(Component::Transform& trans, Anim::RootKeyframe const& root);
}

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

      Component::Transform& trans{ entity.GetComponent<Component::Transform>() };
      Anim::AnimationData const& animData{ am.GetAsset<IGE::Assets::AnimationAsset>(animation.currentAnimation)->mAnimData };

      // if first keyframe, initialize
      if (animation.currentKeyframes.empty()) {
        animation.Reset();
        animation.currentKeyframes = animData.rootKeyframe.nextNodes;
        InitTransform(trans, animData.rootKeyframe);
      }

      float const toBeDeltaTime{ animation.timeElapsed + deltaTime };
      std::queue<Anim::Node> completedNodes{};

      // execute all keyframes in range by setting the interpolated value based on the current t
      for (auto iter{ animation.currentKeyframes.begin() }; iter != animation.currentKeyframes.end();) {
        Anim::Keyframe& keyframe{ **iter };
        // get the current t value mapped to [0, 1]
        float const normalizedTime{ (animation.timeElapsed - keyframe.startTime) / keyframe.duration };

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
          completedNodes.emplace(*iter);
          iter = animation.currentKeyframes.erase(iter);
        }
        else {
          ++iter;
        }
      } // end keyframe for loop

      // process the completed nodes
      while (!completedNodes.empty()) {
        auto& nextNodes{ completedNodes.front()->nextNodes };
        // insert the next nodes into the current keyframes
        if (!nextNodes.empty()) {
          animation.currentKeyframes.insert(animation.currentKeyframes.end(), nextNodes.begin(), nextNodes.end());
        }
        completedNodes.pop();
      }

      animation.timeElapsed = toBeDeltaTime; // update elapsed time

      if (animation.currentKeyframes.empty()) {
        animation.Reset();

        if (!animation.repeat) {
          animation.currentAnimation = {};
        }
      }
    } // end entity for loop
  }

} // namespace Systems

namespace {
  void InitTransform(Component::Transform& trans, Anim::RootKeyframe const& root) {
    trans.position = root.startPos;
    trans.SetLocalRotWithEuler(root.startRot);
    trans.scale = root.startScale;
  }
}