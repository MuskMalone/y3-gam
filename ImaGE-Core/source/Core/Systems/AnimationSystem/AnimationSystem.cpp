#include <pch.h>
#include "AnimationSystem.h"
#include <Core/Entity.h>
#include <Core/Components/Animation.h>
#include <Core/Components/Transform.h>
#include <Asset/IGEAssets.h>
#include <Animation/AnimationData.h>

#include <Events/EventManager.h>
#include <FrameRateController/FrameRateController.h>

namespace {
#ifndef DISTRIBUTION
  struct EntityPreviewData {
    Component::Transform trans;
    IGE::Assets::GUID anim;
    ECS::Entity entity;
  };
#else
  struct EntityPreviewData {};
#endif
  static EntityPreviewData sPreviewData;  // this is for the "Preview" feature, though we don't need this in game build
                                          // @TODO: Find a way to make this editor only

  void InitTransform(Component::Transform& trans, Anim::RootKeyframe const& root);
}

namespace Systems {
  AnimationSystem::AnimationSystem(const char* name) : System(name) {
    SUBSCRIBE_CLASS_FUNC(Events::PreviewAnimation, &AnimationSystem::OnEntityPreview, this);
  }

  void AnimationSystem::Update() {
    float const deltaTime{ IGE_FRC.GetDeltaTime() };
    IGE::Assets::AssetManager& am{ IGE_ASSETMGR };

    for (ECS::Entity entity : mEntityManager.GetAllEntitiesWithComponents<Component::Animation>()) {
      Component::Animation& animation{ entity.GetComponent<Component::Animation>() };

      if (!animation.currentAnimation) { continue; }

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

      animation.timeElapsed += deltaTime; // update elapsed time
      std::queue<Anim::Node> completedNodes{};

      // execute all keyframes in range by setting the interpolated value based on the current t
      for (auto iter{ animation.currentKeyframes.begin() }; iter != animation.currentKeyframes.end();) {
        Anim::Keyframe& keyframe{ **iter };
        // get the current t value mapped to [0, 1]
        float const normalizedTime{ (animation.timeElapsed - keyframe.startTime) / keyframe.duration };

        // if the keyframe ends this frame, complete and remove it
        bool const lastFrame{ animation.timeElapsed >= keyframe.GetEndTime() };

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

      if (animation.currentKeyframes.empty()) {
        animation.Reset();

        if (!animation.repeat) {
          animation.currentAnimation = {};
        }
      }
    } // end entity for loop
  }

  EVENT_CALLBACK_DEF(AnimationSystem, OnEntityPreview) {
    auto ptr{ CAST_TO_EVENT(Events::PreviewAnimation) };
    sPreviewData.entity = ptr->mEntity;
    // keep a copy of the transform to restore afterwards
    sPreviewData.trans = sPreviewData.entity.GetComponent<Component::Transform>();
    sPreviewData.anim = ptr->mGUID;
  }

  // exactly the same as Update(), except we only run entities with "preview" flag set to true
  void AnimationSystem::PausedUpdate() {
    if (!sPreviewData.entity) { return; }

    float const deltaTime{ IGE_FRC.GetDeltaTime() };
    IGE::Assets::AssetManager& am{ IGE_ASSETMGR };

    Component::Animation& animation{ sPreviewData.entity.GetComponent<Component::Animation>() };

    if (!sPreviewData.anim) { return; }

    try {
      am.LoadRef<IGE::Assets::AnimationAsset>(sPreviewData.anim);
    }
    catch (Debug::ExceptionBase&) {
      IGE_DBGLOGGER.LogError("[AnimationSystem] Unable to get animation " +
        std::to_string(static_cast<uint64_t>(sPreviewData.anim)) + " of Entity " + sPreviewData.entity.GetTag());
      sPreviewData.entity = {};
      return;
    }

    Component::Transform& trans{ sPreviewData.entity.GetComponent<Component::Transform>() };
    Anim::AnimationData const& animData{ am.GetAsset<IGE::Assets::AnimationAsset>(sPreviewData.anim)->mAnimData };

    // if first keyframe, initialize
    if (animation.currentKeyframes.empty()) {
      animation.Reset();
      animation.currentKeyframes = animData.rootKeyframe.nextNodes;
      InitTransform(trans, animData.rootKeyframe);
    }

    animation.timeElapsed += deltaTime; // update elapsed time
    std::queue<Anim::Node> completedNodes{};

    // execute all keyframes in range by setting the interpolated value based on the current t
    for (auto iter{ animation.currentKeyframes.begin() }; iter != animation.currentKeyframes.end();) {
      Anim::Keyframe& keyframe{ **iter };
      // get the current t value mapped to [0, 1]
      float const normalizedTime{ (animation.timeElapsed - keyframe.startTime) / keyframe.duration };

      // if the keyframe ends this frame, complete and remove it
      bool const lastFrame{ animation.timeElapsed >= keyframe.GetEndTime() };

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

    if (animation.currentKeyframes.empty()) {
      animation.Reset();
      sPreviewData.entity.EmplaceOrReplaceComponent<Component::Transform>(sPreviewData.trans).modified = true;
      sPreviewData = {};
    }
  }

} // namespace Systems

namespace {
  void InitTransform(Component::Transform& trans, Anim::RootKeyframe const& root) {
    trans.position = root.startPos;
    trans.SetLocalRotWithEuler(root.startRot);
    trans.scale = root.startScale;
  }
}