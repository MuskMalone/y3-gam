#include <pch.h>
#include "LayerSystem.h"

#include "Core/EntityManager.h"
#include "Core/Components/Components.h"

namespace Systems {

  void LayerSystem::Start() {
    SUBSCRIBE_CLASS_FUNC(Events::EventType::LOAD_SCENE, &LayerSystem::OnSceneLoad, this);
  }

  void LayerSystem::Update() {

  }

  bool LayerSystem::IsLayerVisible(std::string layerName) {
    auto itr = std::find(mLayerData.layerNames.begin(), mLayerData.layerNames.end(), layerName);

    if (itr != mLayerData.layerNames.end()) {
      int index = std::distance(mLayerData.layerNames.begin(), itr);
      return mLayerData.layerVisibility[index];
    }

    else {
      Debug::DebugLogger::GetInstance().LogWarning("[Layers] Invalid Layer Name Passed");
    }

    return false;
  }

  void LayerSystem::UpdateEntityLayer(ECS::Entity entity, std::string oldLayer, std::string newLayer) {
    if (oldLayer == newLayer) return;

    auto itr = mLayerEntities.find(oldLayer);
    if (itr != mLayerEntities.end()) {
      std::vector<ECS::Entity>& entities = itr->second;
      auto entityIt = std::find(entities.begin(), entities.end(), entity);

      if (entityIt != entities.end()) {
        entities.erase(entityIt);
      }
    }

    mLayerEntities[newLayer].push_back(entity);
  }

  EVENT_CALLBACK_DEF(LayerSystem, OnSceneLoad) {
    mLayerEntities.clear();
    
    // The built-in layer names should never change
    // This code is necessary as someone might manually edit the built-in layers in the json file...
    mLayerData.layerNames[0] = std::string(BUILTIN_LAYER_0);
    mLayerData.layerNames[1] = std::string(BUILTIN_LAYER_1);

    auto const& allEntities{ ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::Layer>() };

    for (auto const& entity : allEntities) {
      std::string layerName = ECS::Entity{ entity }.GetComponent<Component::Layer>().name;
      
      auto itr = std::find(mLayerData.layerNames.begin(), mLayerData.layerNames.end(), layerName);
      if (itr != mLayerData.layerNames.end()) {
        mLayerEntities[layerName].push_back(ECS::Entity{ entity });
      }

      else {
        std::string tag = ECS::Entity{ entity }.GetComponent<Component::Tag>().tag;
        Debug::DebugLogger::GetInstance().LogWarning("[Layers] The Entity with Tag: \"" + tag + "\" has a Non-Existent Layer");

        // Add entity with non-existent layer into default layer
        mLayerEntities[std::string(BUILTIN_LAYER_0)].push_back(ECS::Entity{ entity });
      }
    }
  }

} // namespace Systems