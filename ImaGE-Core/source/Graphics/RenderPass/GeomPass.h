#pragma once
#include "RenderPass.h"

namespace Graphics {

  class GeomPass : public RenderPass {
  public:
    GeomPass(const RenderPassSpec& spec);

    void RenderSceneToShadowMap();
    void Render(EditorCamera const& camera, std::vector<ECS::Entity> const& entities) override;

  private:

  };

} // namespace Graphics
