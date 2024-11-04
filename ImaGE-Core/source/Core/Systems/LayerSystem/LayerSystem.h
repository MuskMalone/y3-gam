#pragma once
#include <utility>
#include <Core/Systems/System.h>
#include <Core/Entity.h>
#include "Events/EventCallback.h"

namespace Systems {

  constexpr size_t MAX_USER_DEFINED_LAYERS{ 14 };

  constexpr size_t MAX_BUILTIN_LAYERS{ 2 };
  constexpr std::string_view  BUILTIN_LAYER_0{ "Default" };
  constexpr std::string_view  BUILTIN_LAYER_1{ "Player" };

  constexpr size_t MAX_LAYERS{ MAX_USER_DEFINED_LAYERS + MAX_BUILTIN_LAYERS };

  class LayerSystem : public System {
  public:
    struct LayerData {
      std::array<std::string, MAX_LAYERS> layerNames;
      std::array<int, MAX_LAYERS> layerVisibility;
      std::array<std::array<int, MAX_LAYERS>, MAX_LAYERS> collisionMatrix;
    };

  public:
    LayerSystem(const char* name) : System(name), mLayerData{} {}

    void Start() override;
    void Update() override;

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