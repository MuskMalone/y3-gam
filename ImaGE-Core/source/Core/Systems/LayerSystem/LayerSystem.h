#pragma once
#include <utility>
#include <Core/Systems/System.h>
#include <Core/Entity.h>

#include "Events/EventManager.h"
#include <Events/EventCallback.h>

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
    std::array<std::string, MAX_LAYERS>const& GetLayerNames() const { return mLayerData.layerNames; }
    std::array<int, MAX_LAYERS>& GetLayerVisibility() { return mLayerData.layerVisibility; }

    void SetLayerCollisionList(int layerNumber, int layerIndex, bool collisionStatus);
    void SetLayerName(int layerNumber, std::string layerName);

    inline LayerData const& GetLayerData() const noexcept { return mLayerData; }
    inline void LoadLayerData(LayerData&& layerData) { mLayerData = std::move(layerData); }

    bool IsLayerVisible(std::string layerName);
    void UpdateEntityLayer(ECS::Entity entity, std::string oldLayer, std::string newLayer);

  private:
    EVENT_CALLBACK_DECL(OnSceneLoad);

  private:
    LayerData mLayerData;
    std::unordered_map<std::string, std::vector<ECS::Entity>> mLayerEntities;
  };

} // namespace Systems