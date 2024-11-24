#pragma once
#include "RenderPass.h"

namespace ECS { class Entity; }

namespace Graphics {

  class GeomPass : public RenderPass {
  public:
    GeomPass(const RenderPassSpec& spec);

    void Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities) override;

  private:
    using EntityXform = std::pair<ECS::Entity, glm::mat4>;
    using MatGroup = std::vector<EntityXform>;
    using MatGroupsMap = std::unordered_map<uint32_t, MatGroup>;

    struct MaterialGroup {
      int matID;                          // Material ID
      std::shared_ptr<Shader> shader;     // Associated shader
      std::vector<EntityXform> entityPairs; // Vector of entity-transform pairs
    };

    static inline constexpr unsigned sMaxLights = 30;

    std::vector<MaterialGroup> CreateMaterialGroups(std::vector<ECS::Entity> const& entities);
  };

} // namespace Graphics
