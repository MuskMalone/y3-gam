#pragma once
#include "RenderPass.h"
#include <unordered_map>
#include <Core/Entity.h>

namespace ECS { class Entity; }

namespace Graphics {

  class GeomPass : public RenderPass {
  public:
    GeomPass(const RenderPassSpec& spec);

    void Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities) override;

    std::shared_ptr<Framebuffer> GetGameViewFramebuffer() const;
  private:
    struct EntityRenderData {
      EntityRenderData() = default;
      EntityRenderData(glm::mat4 const& _xform, ECS::Entity _entity, int _matIdx) :
        xform{ _xform }, entity{ _entity }, matIdx{ _matIdx } {}

      glm::mat4 xform;
      ECS::Entity entity;
      int matIdx;
    };

    using MatGroupMap = std::unordered_map<int, std::vector<EntityRenderData>>;
    using ShaderGroupMap = std::unordered_map<std::shared_ptr<Shader>, MatGroupMap>;

    static inline constexpr unsigned sMaxLights = 30;

    ShaderGroupMap GroupEntities(std::vector<ECS::Entity> const& entities);

    std::shared_ptr<Framebuffer> mPickFramebuffer{ nullptr };
  };

} // namespace Graphics
