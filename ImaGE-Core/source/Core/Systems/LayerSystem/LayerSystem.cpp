#include <pch.h>
#include "LayerSystem.h"

namespace Systems {

  void LayerSystem::Start() {

  }

  void LayerSystem::Update() {

  }

  void LayerSystem::Destroy() {

  }

  // Events System Scene Change
  void LayerSystem::OnSceneChange() {
    // Deserialize collision matrix and visibility list on scene change
    

    // TEMP
    for (int i{}; i < MAX_LAYERS; ++i) {
      mLayerNames[i] = std::to_string(i);
    }
  }

} // namespace Systems