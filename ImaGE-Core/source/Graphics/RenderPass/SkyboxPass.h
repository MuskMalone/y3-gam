#pragma once
#include "RenderPass.h"

namespace Graphics {

    class SkyboxPass : public RenderPass {
    public:
        SkyboxPass(const RenderPassSpec& spec);
        void Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities) override;
    };

} // namespace Graphics
