#pragma once
#include <Singleton/ThreadSafeSingleton.h>
#include "Events/EventCallback.h"
#include <utility>
#include <Core/Entity.h>

#define IGE_LAYERMGR Layers::LayerManager::GetInstance()

namespace Layers {
  constexpr size_t MAX_USER_DEFINED_LAYERS{ 13 };

  constexpr size_t MAX_BUILTIN_LAYERS{ 3 };
  constexpr std::string_view  BUILTIN_LAYER_0{ "Default" };
  constexpr std::string_view  BUILTIN_LAYER_1{ "Player" };
  constexpr std::string_view  BUILTIN_LAYER_2{ "UI" };

  constexpr size_t MAX_LAYERS{ MAX_USER_DEFINED_LAYERS + MAX_BUILTIN_LAYERS };

  class LayerManager : public ThreadSafeSingleton<LayerManager> {
  public:
    struct LayerData {
      std::array<std::string, MAX_LAYERS> layerNames;
      std::array<int, MAX_LAYERS> layerVisibility;
      std::array<std::array<int, MAX_LAYERS>, MAX_LAYERS> collisionMatrix;
    };

  public:
    LayerManager();

    void CopyValidEntities(std::vector<ECS::Entity>& entityVector, const std::pair<std::string, std::vector<ECS::Entity>>& mapPair);

    std::array<int, MAX_LAYERS>const& GetLayerCollisionList(int layerNumber) const;
    inline std::array<std::string, MAX_LAYERS>const& GetLayerNames() const { return mLayerData.layerNames; }
    inline std::array<int, MAX_LAYERS>& GetLayerVisibility() { return mLayerData.layerVisibility; }
    inline LayerData const& GetLayerData() const noexcept { return mLayerData; }
    inline std::unordered_map<std::string, std::vector<ECS::Entity>> const& GetLayerEntities() const { return mLayerEntities; }
    inline bool GetCollidable(int lhsLayerNumber, int rhsLayerNumber) const noexcept {
      return mLayerData.collisionMatrix[lhsLayerNumber][rhsLayerNumber];
    }

    void SetLayerCollisionList(int layerNumber, int layerIndex, bool collisionStatus);
    void SetLayerName(int layerNumber, std::string layerName);
    inline void LoadLayerData(LayerData&& layerData) { mLayerData = std::move(layerData); }

    bool IsLayerVisible(std::string layerName);
    void UpdateEntityLayer(ECS::Entity entity, std::string oldLayer, std::string newLayer);

    physx::PxFilterFlags LayerFilterShader(
      physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
      physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
      physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize) const;

    void SetupShapeFilterData(physx::PxShape** shape, ECS::Entity entity);

  private:
    EVENT_CALLBACK_DECL(OnSceneChange);
    EVENT_CALLBACK_DECL(OnLayerModification);
    EVENT_CALLBACK_DECL(OnPrefabEditor);
    EVENT_CALLBACK_DECL(OnEntityRemove);

  private:
    LayerData mLayerData;
    std::unordered_map<std::string, std::vector<ECS::Entity>> mLayerEntities;
  };

} // namespace Systems