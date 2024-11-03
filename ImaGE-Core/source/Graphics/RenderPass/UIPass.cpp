#include "pch.h"
#include "UIPass.h"

namespace Graphics {
	UIPass::UIPass(const RenderPassSpec& spec) : RenderPass{ spec } {

	}

	void UIPass::Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities){
		Begin();
		//Renderer::Clear();

		auto const& shader{ mSpec.pipeline->GetShader() };

		//auto const& fbSpec{ mSpec.pipeline->GetSpec().targetFramebuffer->GetFramebufferSpec()};
		////create ortho proj matrix
		//float width = static_cast<float>(fbSpec.width);
		//float height = static_cast<float>(fbSpec.height);

		glm::mat4 viewProj{};

		if (cam.isEditor) {
			viewProj = cam.viewProjMatrix;
		}
		else {
			viewProj = Renderer::mUICamera.GetViewProjMatrix();
		} 

		shader->SetUniform("u_ViewProjMtx", viewProj);

		Renderer::BeginBatch(); //scene begin

		for (ECS::Entity const& entity : entities) {
			if (!entity.HasComponent<Component::Canvas>()) { continue; } //if not canvas skip

			//canvas found

			ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
			auto const& children{ entityMan.GetChildEntity(entity) }; //vector of UI element entity
			auto const& xform = entity.GetComponent<Component::Transform>(); //canvas xform


			//Graphics::Renderer::SubmitInstance(Renderer::GetQuadMeshSource(), xform.worldMtx, Color::COLOR_BLUE, entity.GetEntityID()); //canvas

			for (ECS::Entity const& uiEntity : children) {
				auto const& uiXform = uiEntity.GetComponent<Component::Transform>(); //ui element transform in screen space
				if (uiEntity.HasComponent<Component::Text>()) {
					auto const& textComp = uiEntity.GetComponent<Component::Text>();
					//Draw Text here
				}
				else if (uiEntity.HasComponent<Component::Image>()) {
					auto const& imageComp = uiEntity.GetComponent<Component::Image>();
					Renderer::DrawQuad(uiXform.position, glm::vec2{ uiXform.scale }, uiXform.rotation, imageComp.color); //TO CHANGE TO SPRITE VERSION
				}
				else {
					Renderer::DrawQuad(uiXform.position, glm::vec2{ uiXform.scale }, uiXform.rotation, Color::COLOR_WHITE);
				}
				

				//Graphics::Renderer::SubmitInstance(Renderer::GetQuadMeshSource(), uiXform.worldMtx, Color::COLOR_WHITE, uiEntity.GetEntityID());
			}
		}
		Renderer::RenderSceneEnd();
		//Renderer::RenderInstances();


		//auto const& shader = mSpec.pipeline->GetShader();
		//shader->SetUniform("u_ScreenTex", mInputTexture);
		//Renderer::RenderFullscreenTexture();

		End();
	}
}

