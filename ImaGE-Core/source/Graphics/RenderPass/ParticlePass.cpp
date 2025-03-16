#include <pch.h>
#include "ParticlePass.h"
#include "Graphics/PostProcessing/PostProcessingManager.h"
#include "Graphics/PostProcessing/ParticleManager.h"
#include "Graphics/RenderPass/PostProcessPass.h"
#include "Graphics/RenderPass/GeomPass.h"
#include "Graphics/Renderer.h"

namespace Graphics {
	ParticlePass::ParticlePass(const RenderPassSpec& spec) : RenderPass{ spec }
	{
	}
	void ParticlePass::Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities)
	{
		//renders out the default view first
		mSpec.pipeline->GetSpec().shader = PostProcessingManager::GetInstance().GetDefaultShader();
		Begin();
		mSpec.pipeline->GetSpec().shader->SetUniform("u_ScreenTex", mInputTexture, 0);
		Renderer::RenderFullscreenTexture();

		End();

		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		mSpec.pipeline->GetSpec().shader = ShaderLibrary::Get("Particle");
		Begin();

		auto const& shader = mSpec.pipeline->GetShader();
		//manual depth buffer comparison just cuz
		shader->SetUniform("depthBuffer", Renderer::GetPass<GeomPass>()->GetDepthTexture(), 0);
		shader->SetUniform("vertViewProjection", cam.viewProjMatrix);
		shader->SetUniform("vertView", cam.viewMatrix);
		glDrawArrays(GL_POINTS, 0, MAX_BUFFER);

		End();
		// Check if mOutputTexture is null or if dimensions don’t match

		auto const& fb = mSpec.pipeline->GetSpec().targetFramebuffer;
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
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