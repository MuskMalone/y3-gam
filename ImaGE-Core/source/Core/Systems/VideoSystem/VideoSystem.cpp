#include "pch.h"
#include "VideoSystem.h"
#include <Core/Entity.h>
#include <Core/Components/Video.h>
#include <FrameRateController/FrameRateController.h>

namespace Systems {
  VideoSystem::VideoSystem(const char* name) : System(name) {

  }

  void VideoSystem::Update() {
    for (ECS::Entity entity : mEntityManager.GetAllEntitiesWithComponents<Component::Video>()) {
      if (!entity.IsActive()) { continue; }

      Component::Video& video{ entity.GetComponent<Component::Video>() };
      if (!video.guid || video.paused || video.HasVideoEnded()) { continue; }
      if (!video.playOnStart && !video.started) { continue; }

      float const initialTimestamp{ video.timeElapsed };
      // video component will handle the rest
      video.AdvanceVideo(IGE_FRC.GetDeltaTime());
    }
  }
} // namespace Systems
