#include <pch.h>
#include "LayerSystem.h"

#include "Core/EntityManager.h"
#include "Core/Components/Components.h"

#include "Physics/PhysicsSystem.h"

namespace Systems {

  void LayerSystem::Start() {
    SUBSCRIBE_CLASS_FUNC(Events::EventType::LOAD_SCENE, &LayerSystem::OnSceneLoad, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::LAYER_MODIFIED, &LayerSystem::OnLayerModification, this);
  }

  void LayerSystem::Update() {

  }

  std::array<int, MAX_LAYERS> const& LayerSystem::GetLayerCollisionList(int layerNumber) const {
    if (layerNumber >= MAX_LAYERS || layerNumber < 0) {
      Debug::DebugLogger::GetInstance().LogWarning("[Layers] Invalid Layer Number Passed");
      return std::array<int, MAX_LAYERS>();
    }

    return mLayerData.collisionMatrix[layerNumber];
  }

  void LayerSystem::SetLayerCollisionList(int layerNumber, int layerIndex, bool collisionStatus) {
    if (layerNumber >= MAX_LAYERS || layerNumber < 0) {
      Debug::DebugLogger::GetInstance().LogWarning("[Layers] Invalid Layer Number Passed");
      return;
    }

    if (layerIndex >= MAX_LAYERS || layerIndex < 0) {
      Debug::DebugLogger::GetInstance().LogWarning("[Layers] Invalid Layer Index Passed");
      return;
    }

    mLayerData.collisionMatrix[layerNumber][layerIndex] = static_cast<int>(collisionStatus);
    // Set the collision matrix of the corresponding layer for symmetric collision
    mLayerData.collisionMatrix[layerIndex][layerNumber] = static_cast<int>(collisionStatus);
  }

  void LayerSystem::SetLayerName(int layerNumber, std::string layerName) {
    if (layerNumber >= MAX_LAYERS || layerNumber < 0) {
      Debug::DebugLogger::GetInstance().LogWarning("[Layers] Invalid Layer Number Passed");
      return;
    }

    mLayerData.layerNames[layerNumber] = layerName;
  }

  bool LayerSystem::IsLayerVisible(std::string layerName) {
    auto itr = std::find(mLayerData.layerNames.begin(), mLayerData.layerNames.end(), layerName);

    if (itr != mLayerData.layerNames.end()) {
      size_t index = static_cast<int>(std::distance(mLayerData.layerNames.begin(), itr));
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

  physx::PxFilterFlags LayerSystem::LayerFilterShader(physx::PxFilterObjectAttributes attributes0, 
    physx::PxFilterData filterData0, physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, 
    physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize) const {

    physx::PxU32 layer0 = filterData0.word0;
    physx::PxU32 layer1 = filterData1.word0;

    // Collision not allowed
    if (!GetCollidable(layer0, layer1)) {
      return physx::PxFilterFlag::eSUPPRESS;
    }

    // Collision allowed
    pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;
    return physx::PxFilterFlag::eDEFAULT;
  }

  void LayerSystem::SetupShapeFilterData(physx::PxShape** shape, ECS::Entity entity) {
    auto itr = std::find(mLayerData.layerNames.begin(), mLayerData.layerNames.end(), 
      entity.GetComponent<Component::Layer>().name);

    if (itr != mLayerData.layerNames.end()) {
      size_t index = std::distance(mLayerData.layerNames.begin(), itr);

      physx::PxFilterData filterData;
      filterData.word0 = static_cast<physx::PxU32>(index);
      /*
      bool isActive = entity.IsActive();
      filterData.word1 = isActive;
      */
      (*shape)->setSimulationFilterData(filterData);
    }
    else {
      Debug::DebugLogger::GetInstance().LogWarning("[Layers] Entiy does not have Valid Layer");
    }
  }

  EVENT_CALLBACK_DEF(LayerSystem, OnSceneLoad) {
    mLayerEntities.clear();
    
    // The built-in layer names should never change
    // This code is necessary as someone might manually edit the built-in layers in the json file...
    mLayerData.layerNames[0] = std::string(BUILTIN_LAYER_0);
    mLayerData.layerNames[1] = std::string(BUILTIN_LAYER_1);

    auto const& allEntities{ ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::Layer>() };

    for (auto const& entity : allEntities) {
      std::string const layerName = ECS::Entity{ entity }.GetComponent<Component::Layer>().name;
      
      auto itr = std::find(mLayerData.layerNames.begin(), mLayerData.layerNames.end(), layerName);
      if (itr != mLayerData.layerNames.end()) {
        mLayerEntities[layerName].push_back(ECS::Entity{ entity });
      }

      else {
        std::string const tag = ECS::Entity{ entity }.GetComponent<Component::Tag>().tag;
        Debug::DebugLogger::GetInstance().LogWarning("[Layers] The Entity with Tag: \"" + tag + "\" has a Non-Existent Layer");

        // Add entity with non-existent layer into default layer
        mLayerEntities[std::string(BUILTIN_LAYER_0)].push_back(ECS::Entity{ entity });
        ECS::Entity{ entity }.GetComponent<Component::Layer>().name = std::string(BUILTIN_LAYER_0);
      }
    }
  }

  EVENT_CALLBACK_DEF(LayerSystem, OnLayerModification) {
    auto layerModifiedEvent{ std::static_pointer_cast<Events::EntityLayerModified>(event) };
    ECS::Entity entity = layerModifiedEvent->mEntity;
    std::string oldLayer = layerModifiedEvent->mOldLayer;

    // Change layer in mLayerEntities
    std::string newLayer = entity.GetComponent<Component::Layer>().name;
    auto itr = std::find(mLayerData.layerNames.begin(), mLayerData.layerNames.end(), newLayer);
    if (itr == mLayerData.layerNames.end()) {
      std::string const tag = ECS::Entity{ entity }.GetComponent<Component::Tag>().tag;
      Debug::DebugLogger::GetInstance().LogWarning("[Layers] The Entity with Tag: \"" + tag + "\" has a Non-Existent Layer");
      newLayer = std::string(BUILTIN_LAYER_0);
    }

    // Remove previous
    auto mapItr = mLayerEntities.find(oldLayer);
    if (mapItr != mLayerEntities.end()) {
      std::vector<ECS::Entity>& entities = mapItr->second;
      entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
    }

    // Add new
    mLayerEntities[newLayer].push_back(entity);
    /*
    mapItr = mLayerEntities.find(newLayer);
    std::vector<ECS::Entity>& newLayerEntities = mapItr->second;
    newLayerEntities.push_back(entity);
    */

    // For updating physics object's filter data (layer collision detection)
    if (!entity.HasComponent<Component::Transform, Component::RigidBody>()) {
      return;
    }

    if (std::shared_ptr<IGE::Physics::PhysicsSystem> physicsSys =
      Systems::SystemManager::GetInstance().GetSystem<IGE::Physics::PhysicsSystem>().lock()) {
      auto& xfm{ entity.GetComponent<Component::Transform>() };
      auto& rb{ entity.GetComponent<Component::RigidBody>() };
      auto const& rigidBodyMap = physicsSys->GetRigidBodyIDs();
      auto rbiter{ rigidBodyMap.find(rb.bodyID) };
      if (rbiter != rigidBodyMap.end()) {
        physx::PxRigidDynamic* pxrigidbody{ rigidBodyMap.at(rb.bodyID) };
        physx::PxShape* shape;
        pxrigidbody->getShapes(&shape, 1);
        SetupShapeFilterData(&shape, entity);
      }
    }
  }

} // namespace Systems