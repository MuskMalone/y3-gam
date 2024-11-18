#pragma once
#include "RenderPass.h"

namespace Graphics {

  class GeomPass : public RenderPass {
  public:
    GeomPass(const RenderPassSpec& spec);

    void Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities) override;

  private:
    static inline constexpr unsigned sMaxLights = 30;
  };

} // namespace Graphics
