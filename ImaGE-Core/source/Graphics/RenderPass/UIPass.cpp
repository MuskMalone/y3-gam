#include "pch.h"
#include "UIPass.h"

namespace Graphics {
	UIPass::UIPass(const RenderPassSpec& spec) : RenderPass{ spec } {

	}

	void UIPass::Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities){
		Begin();

		//auto const& shader = mSpec.pipeline->GetShader();
		//shader->SetUniform("u_ScreenTex", mInputTexture);
		//Renderer::RenderFullscreenTexture();

		End();
	}
}

