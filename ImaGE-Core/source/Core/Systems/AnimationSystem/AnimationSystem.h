#pragma once
#include <Core/Systems/System.h>

namespace Systems {
  class AnimationSystem : public System {
  public:
    AnimationSystem(const char* name) : System(name) {}

    /*!*********************************************************************
    \brief
      Update function of the system
    ************************************************************************/
    void Update() override;

  private:
  };
}
