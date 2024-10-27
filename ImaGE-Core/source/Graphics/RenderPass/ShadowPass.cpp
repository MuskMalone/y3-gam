#include <pch.h>
#include "ShadowPass.h"
#include "Core/Entity.h"
#include <Graphics/EditorCamera.h>

namespace Graphics {

  ShadowPass::ShadowPass(const RenderPassSpec& spec) : RenderPass(spec) {

  }

  void ShadowPass::Render(EditorCamera const& cam, std::vector<ECS::Entity> const& entities) {
    Begin();

    

    End();
  }

} // namespace Graphics
