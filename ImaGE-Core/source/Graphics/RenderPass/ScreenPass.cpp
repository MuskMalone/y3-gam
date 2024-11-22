#include "pch.h"
#include "ScreenPass.h"
#include "Graphics/Renderer.h"

namespace Graphics {
	ScreenPass::ScreenPass(const RenderPassSpec& spec) : RenderPass{ spec } {

	}

	void ScreenPass::Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities) {
		Begin();
		Renderer::Clear();

		auto const& shader = mSpec.pipeline->GetShader();
		shader->SetUniform("u_ScreenTex", mInputTexture, 0);
		Renderer::RenderFullscreenTexture();

		End();
	}
}

