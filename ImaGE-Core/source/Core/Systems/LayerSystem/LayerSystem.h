#pragma once
#include <utility>
#include <Core/Systems/System.h>
#include <Core/Entity.h>

namespace Systems {

  constexpr size_t MAX_USER_DEFINED_LAYERS = 14;
  constexpr size_t MAX_BUILTIN_LAYERS = 2;
  constexpr size_t MAX_LAYERS = MAX_USER_DEFINED_LAYERS + MAX_BUILTIN_LAYERS;

  class LayerSystem : public System {
  public:
    // doing this so i can just pass the
    // whole struct to serialize
    struct LayerData {
      std::array<std::string, MAX_LAYERS> layerNames;
      std::array<int, MAX_LAYERS> layerVisibility;
      std::array<std::array<int, MAX_LAYERS>, MAX_LAYERS> collisionMatrix;
    };

  public:
    LayerSystem(const char* name) : System(name), mLayerData{} {}

    void Start() override;
    void Update() override;
    void Destroy() override;

    void OnSceneChange();

    std::array<std::string, MAX_LAYERS>const& GetLayerNames() const { return mLayerData.layerNames; }
    std::array<int, MAX_LAYERS>& GetLayerVisibility() { return mLayerData.layerVisibility; }

    inline LayerData const& GetLayerData() const noexcept { return mLayerData; }
    inline void LoadLayerData(LayerData&& layerData) { mLayerData = std::move(layerData); }

  private:
    LayerData mLayerData;

    std::unordered_map<std::string, std::vector<ECS::Entity>> mLayerEntities;
  };

} // namespace Systems