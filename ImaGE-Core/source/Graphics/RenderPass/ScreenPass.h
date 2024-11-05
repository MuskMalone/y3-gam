#pragma once
#include "RenderPass.h"

namespace Graphics {

    class ScreenPass : public RenderPass {
    public:
        ScreenPass(const RenderPassSpec& spec);
        void Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities) override;
    };

} // namespace Graphics
