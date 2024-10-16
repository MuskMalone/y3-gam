#include <pch.h>
#include "LayerSystem.h"

namespace Systems {

  // Static Initialization
  std::array<std::string, MAX_LAYERS> LayerSystem::mLayerNames;
  std::array<int, MAX_LAYERS> LayerSystem::mLayerVisibility;
  std::array<std::array<int, MAX_LAYERS>, MAX_LAYERS> LayerSystem::mCollisionMatrix;
  std::unordered_map<std::string, std::vector<ECS::Entity>> LayerSystem::mLayerEntities;

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