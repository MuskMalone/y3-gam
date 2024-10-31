#pragma once
#include "RenderPass.h"

namespace Graphics {

    class UIPass : public RenderPass {
    public:
        UIPass(const RenderPassSpec& spec);

        void Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities) override;

    };

} // namespace Graphics
