#pragma once
#include "RenderPass.h"

namespace Graphics {

  class GeomPass : public RenderPass {
  public:
    GeomPass(const RenderPassSpec& spec);

    void RenderSceneToShadowMap();
    void Render(EditorCamera const& camera) override;

  private:

  };

} // namespace Graphics
