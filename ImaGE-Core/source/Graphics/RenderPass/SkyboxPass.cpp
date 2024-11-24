#include "pch.h"
#include "SkyboxPass.h"
#include "Graphics/Renderer.h"
#include "Asset/IGEAssets.h"
#include <Core/Components/Components.h>

namespace Graphics {
	SkyboxPass::SkyboxPass(const RenderPassSpec& spec) : RenderPass{ spec } {

	}

	void SkyboxPass::Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities) {
		Begin();
		Renderer::Clear();
		std::shared_ptr<Shader> shader = mSpec.pipeline->GetShader(); //default using Proc
		glm::mat4 invViewProjMatrix = glm::inverse(cam.viewProjMatrix);

		ECS::Entity skyboxEnt{};
		for (auto const& entity : entities) {
			if (!entity.HasComponent<Component::Skybox>()) continue;
			if (!entity.HasComponent<Component::Camera>()) continue;

			skyboxEnt = entity; break;
		}

		if (skyboxEnt != ECS::Entity{}) {
			auto const& skybox = skyboxEnt.GetComponent<Component::Skybox>();
			shader = ShaderLibrary::Get("SkyboxPano");
			shader->Use();

			if (skybox.tex1.IsValid())
				shader->SetUniform("u_Panoramic1", IGE_ASSETMGR.GetAsset<IGE::Assets::TextureAsset>(skybox.tex1)->mTexture, 0);
			if(skybox.tex2.IsValid())
				shader->SetUniform("u_Panoramic2", IGE_ASSETMGR.GetAsset<IGE::Assets::TextureAsset>(skybox.tex2)->mTexture, 1);
			shader->SetUniform("u_Blend", skybox.blend);
		}

		shader->SetUniform("u_InvViewProj", invViewProjMatrix);
		shader->SetUniform("u_Exposure", 1.f);

		glDepthMask(GL_FALSE); // Disable depth writes for background
		Renderer::RenderFullscreenTexture();
		glDepthMask(GL_TRUE);
		End();

	}
}

