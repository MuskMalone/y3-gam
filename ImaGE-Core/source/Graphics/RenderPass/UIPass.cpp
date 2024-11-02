#include "pch.h"
#include "UIPass.h"

namespace Graphics {
	UIPass::UIPass(const RenderPassSpec& spec) : RenderPass{ spec } {

	}

	void UIPass::Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities){
		Begin();
		//Renderer::Clear();
		auto const& shader{ mSpec.pipeline->GetShader() };

		auto const& fbSpec{ mSpec.pipeline->GetSpec().targetFramebuffer->GetFramebufferSpec()};
		//create ortho proj matrix
		int w = static_cast<float>(fbSpec.width);
		int h = static_cast<float>(fbSpec.height);
		glm::mat4 orthoProj = glm::ortho(0.f, 1920.f, 1080.f, 0.f, -1.f, 1.f);

		shader->SetUniform("u_ViewProjMtx", orthoProj);
        for (ECS::Entity const& entity : entities) {
            if (!entity.HasComponent<Component::Canvas>()) { continue; } //if not canvas skip

			//canvas found

			ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
			auto const& children{ entityMan.GetChildEntity(entity) }; //vector of UI element entity
            auto const& xform = entity.GetComponent<Component::Transform>(); //canvas xform


			Graphics::Renderer::SubmitInstance(Renderer::GetQuadMeshSource(), xform.worldMtx, Color::COLOR_BLUE, entity.GetEntityID());

			for (ECS::Entity const& uiEntity : children) {
				auto const& uiXform = uiEntity.GetComponent<Component::Transform>(); //ui element transform in screen space
				
				Graphics::Renderer::SubmitInstance(Renderer::GetQuadMeshSource(), uiXform.worldMtx, Color::COLOR_WHITE, uiEntity.GetEntityID());
			}
        }

		Renderer::RenderInstances();
		//auto const& shader = mSpec.pipeline->GetShader();
		//shader->SetUniform("u_ScreenTex", mInputTexture);
		//Renderer::RenderFullscreenTexture();

		End();
	}
}

