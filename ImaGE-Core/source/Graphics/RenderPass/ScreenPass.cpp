#include "pch.h"
#include "ScreenPass.h"

namespace Graphics {
	ScreenPass::ScreenPass(const RenderPassSpec& spec) : RenderPass{ spec } {

	}

	void ScreenPass::Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities) {
		Begin();

		auto const& shader = mSpec.pipeline->GetShader();
		shader->SetUniform("u_ScreenTex", mInputTexture);
		Renderer::RenderFullscreenTexture();

		End();
	}
}

