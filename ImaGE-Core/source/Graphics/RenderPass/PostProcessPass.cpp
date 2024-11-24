#include <pch.h>
#include "PostProcessPass.h"
#include "Graphics/PostProcessing/PostProcessingManager.h"
#include "FrameRateController/FrameRateController.h"
#include "Input/InputManager.h"
#include "Graphics/Renderer.h"
namespace Graphics {
	PostProcessingPass::PostProcessingPass(const RenderPassSpec& spec) : RenderPass{ spec } {
		Graphics::FramebufferSpec postprocessSpec;
		postprocessSpec.width = WINDOW_WIDTH<int>;
		postprocessSpec.height = WINDOW_HEIGHT<int>;
		postprocessSpec.attachments = { Graphics::FramebufferTextureFormat::RGBA8 };	// temporarily max. 1 shadow-caster

		mPingPongBuffer = Framebuffer::Create(postprocessSpec);
	}

	void PostProcessingPass::Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities)
	{
		if (!cam.isEditor) {
			{//fog/visibility shader
				//copy the color buffer to inputTexture
				glMemoryBarrier(GL_ALL_BARRIER_BITS);
				auto shader{ ShaderLibrary::Get("Fog") };
				mSpec.pipeline->GetSpec().shader = shader;
				Begin();
				shader->SetUniform("u_TexViewPosition", mPositionGBuffer, 0);
				shader->SetUniform("u_TexFragColor", mInputTexture, 1);

				shader->SetUniform("u_MinDist", Graphics::PostProcessingManager::GetInstance().GetFogMinDist());
				shader->SetUniform("u_MaxDist", Graphics::PostProcessingManager::GetInstance().GetFogMaxDist());
				shader->SetUniform("u_FogColor", Graphics::PostProcessingManager::GetInstance().GetFogColor());
				Renderer::RenderFullscreenTexture();
				//swap back the shader
				End();

				auto const& fb = mSpec.pipeline->GetSpec().targetFramebuffer;

				// Check if mOutputTexture is null or if dimensions don’t match
				if (mInputTexture) {
					mInputTexture->CopyFrom(fb->GetColorAttachmentID(), fb->GetFramebufferSpec().width, fb->GetFramebufferSpec().height);
				}
				std::swap(mSpec.pipeline->GetSpec().targetFramebuffer, mPingPongBuffer);
				glMemoryBarrier(GL_ALL_BARRIER_BITS);
			}
			auto numShaders{ Graphics::PostProcessingManager::GetInstance().GetShaderNum() };
			numShaders = (numShaders) ? numShaders : 1;
			for (unsigned i{}; i < numShaders; ++i) {
				mSpec.pipeline->GetSpec().shader = Graphics::PostProcessingManager::GetInstance().GetShader(i);
				Begin();

				auto const& shader = mSpec.pipeline->GetShader();
				shader->SetUniform("u_Resolution", glm::vec2(
					mSpec.pipeline->GetSpec().targetFramebuffer->GetFramebufferSpec().width,
					mSpec.pipeline->GetSpec().targetFramebuffer->GetFramebufferSpec().height
				));
				shader->SetUniform("u_Time", static_cast<float>(glfwGetTime()));
				shader->SetUniform("u_Frame", static_cast<int>(Performance::FrameRateController::GetInstance().GetFrameCounter()));
				shader->SetUniform("u_Mouse", Input::InputManager::GetInstance().GetMousePos());
				shader->SetUniform("u_ScreenTex", mInputTexture, 0);

				Renderer::RenderFullscreenTexture();
				End();
				auto const& fb = mSpec.pipeline->GetSpec().targetFramebuffer;

				// Check if mOutputTexture is null or if dimensions don’t match
				if (i < numShaders - 1) {
					if (mInputTexture) {
						mInputTexture->CopyFrom(fb->GetColorAttachmentID(), fb->GetFramebufferSpec().width, fb->GetFramebufferSpec().height);
					}
					std::swap(mSpec.pipeline->GetSpec().targetFramebuffer, mPingPongBuffer);
				}
				else {
					if (!mOutputTexture || mOutputTexture->GetWidth() != fb->GetFramebufferSpec().width || mOutputTexture->GetHeight() != fb->GetFramebufferSpec().height) {
						// Create or resize mOutputTexture based on the framebuffer's specs
						mOutputTexture = std::make_shared<Graphics::Texture>(fb->GetFramebufferSpec().width, fb->GetFramebufferSpec().height);
					}

					// Perform the copy operation
					if (mOutputTexture) {
						mOutputTexture->CopyFrom(fb->GetColorAttachmentID(), fb->GetFramebufferSpec().width, fb->GetFramebufferSpec().height);
					}
				}
				glMemoryBarrier(GL_ALL_BARRIER_BITS);

			}
		}
		else {
			mSpec.pipeline->GetSpec().shader = Graphics::PostProcessingManager::GetInstance().GetDefaultShader();
			Begin();

			auto const& shader = mSpec.pipeline->GetShader();
			shader->SetUniform("u_ScreenTex", mInputTexture, 0);

			Renderer::RenderFullscreenTexture();
			End();
			auto const& fb = mSpec.pipeline->GetSpec().targetFramebuffer;
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
}