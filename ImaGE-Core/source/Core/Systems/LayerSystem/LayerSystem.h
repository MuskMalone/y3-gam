#pragma once
#include <Core/Systems/System.h>
#include <Core/Entity.h>

namespace Systems {

  constexpr size_t MAX_USER_DEFINED_LAYERS = 14;
  constexpr size_t MAX_BUILTIN_LAYERS = 2;
  constexpr size_t MAX_LAYERS = MAX_USER_DEFINED_LAYERS + MAX_BUILTIN_LAYERS;

  class LayerSystem : public System {
  public:
    LayerSystem(const char* name) : System(name) {}

    void Start() override;
    void Update() override;
    void Destroy() override;

    void OnSceneChange();

    std::array<std::string, MAX_LAYERS>const& GetLayerNames() { return mLayerNames; }
    std::array<int, MAX_LAYERS>& GetLayerVisibility() { return mLayerVisibility; }

  private:
    std::array<std::string, MAX_LAYERS> mLayerNames;
    std::array<int, MAX_LAYERS> mLayerVisibility;
    std::array<std::array<int, MAX_LAYERS>, MAX_LAYERS> mCollisionMatrix;

    std::unordered_map<std::string, std::vector<ECS::Entity>> mLayerEntities;
  };

} // namespace Systems