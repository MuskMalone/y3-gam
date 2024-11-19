#include "pch.h"
#include "SkyboxPass.h"
#include "Graphics/Renderer.h"
#include "Asset/IGEAssets.h"
namespace Graphics {
	SkyboxPass::SkyboxPass(const RenderPassSpec& spec) : RenderPass{ spec } {

	}

	void SkyboxPass::Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities) {
		Begin();
		Renderer::Clear();

		auto const& shader = mSpec.pipeline->GetShader();

		glm::mat4 invViewProjMatrix = glm::inverse(cam.viewProjMatrix);

		shader->SetUniform("u_InvViewProj", invViewProjMatrix);
		shader->SetUniform("u_Panoramic", IGE_ASSETMGR.GetAsset<IGE::Assets::TextureAsset>(Renderer::mSkyboxTextures[0])->mTexture);
		shader->SetUniform("u_Exposure", 1.f);
		glDepthMask(GL_FALSE); // Disable depth writes for background
		Renderer::RenderFullscreenTexture();
		glDepthMask(GL_TRUE);
		End();

	}
}

