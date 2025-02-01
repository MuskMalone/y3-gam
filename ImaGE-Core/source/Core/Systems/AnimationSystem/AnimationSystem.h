#pragma once
#include <Core/Systems/System.h>
#include <Events/EventCallback.h>

namespace Systems {
  class AnimationSystem : public System {
  public:
    AnimationSystem(const char* name);

    void Update() override;
    void PausedUpdate() override;

  private:
    EVENT_CALLBACK_DECL(OnEntityPreview);
  };
}
