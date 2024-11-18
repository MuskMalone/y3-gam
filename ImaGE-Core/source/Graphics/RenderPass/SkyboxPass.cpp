#include "pch.h"
#include "SkyboxPass.h"
#include "Graphics/Renderer.h"

namespace Graphics {
	SkyboxPass::SkyboxPass(const RenderPassSpec& spec) : RenderPass{ spec } {

	}

	void SkyboxPass::Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities) {
		Begin();
		Renderer::Clear();

		auto const& shader = mSpec.pipeline->GetShader();
		shader->SetUniform("u_ScreenTex", mInputTexture);
		Renderer::RenderFullscreenTexture();

		End();

		auto const& fb = mSpec.pipeline->GetSpec().targetFramebuffer;

		// Check if mOutputTexture is null or if dimensions don’t match
		if (!mOutputTexture || mOutputTexture->GetWidth() != fb->GetFramebufferSpec().width || mOutputTexture->GetHeight() != fb->GetFramebufferSpec().height) {
			// Create or resize mOutputTexture based on the framebuffer's specs
			mOutputTexture = std::make_shared<Graphics::Texture>(fb->GetFramebufferSpec().width, fb->GetFramebufferSpec().height);
		}

		// Perform the copy operation
		if (mOutputTexture) {
			mOutputTexture->CopyFrom(fb->GetColorAttachmentID(), fb->GetFramebufferSpec().width, fb->GetFramebufferSpec().height);
		}
	}
}

