#pragma once
#include "RenderPass.h"

namespace Graphics {

  class ShadowPass : public RenderPass {
  public:
    ShadowPass(const RenderPassSpec& spec);

    void Render(EditorCamera const& camera, std::vector<ECS::Entity> const& entities) override;

  private:

  };

} // namespace Graphics
