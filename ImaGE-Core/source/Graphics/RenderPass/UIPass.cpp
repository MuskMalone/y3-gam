#include "pch.h"
#include "UIPass.h"

namespace Graphics {
	UIPass::UIPass(const RenderPassSpec& spec) : RenderPass{ spec } {

	}

	void UIPass::Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities){

		auto const& shader{ mSpec.pipeline->GetShader() };

		Begin();
		//Renderer::Clear();

		//auto const& fbSpec{ mSpec.pipeline->GetSpec().targetFramebuffer->GetFramebufferSpec()};
		////create ortho proj matrix
		//float width = static_cast<float>(fbSpec.width);
		//float height = static_cast<float>(fbSpec.height);

		Renderer::BeginBatch(); //scene begin

		glm::mat4 viewProj{};
		if (cam.isEditor) {
			viewProj = cam.viewProjMatrix;
		}
		else {
			viewProj = Renderer::mUICamera.GetViewProjMatrix();
		}

		// @TODO: TEMP, TO MERGE WITH XAVIER
		shader->Unuse();
		if (std::shared_ptr<Systems::TextSystem> textSys =
			Systems::SystemManager::GetInstance().GetSystem<Systems::TextSystem>().lock()) {
			textSys->RenderTextForAllEntities(viewProj);
		}
		shader->Use();

		shader->SetUniform("u_ViewProjMtx", viewProj);

		for (ECS::Entity const& entity : entities) {
			if (!entity.HasComponent<Component::Canvas>()) { continue; } //if not canvas skip

			//canvas found

			ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
			auto children{ entityMan.GetChildEntity(entity) }; //vector of UI element entity
			auto const& xform = entity.GetComponent<Component::Transform>(); //canvas xform


			//Graphics::Renderer::SubmitInstance(Renderer::GetQuadMeshSource(), xform.worldMtx, Color::COLOR_BLUE, entity.GetEntityID()); //canvas

			for (ECS::Entity& uiEntity : children) {
				auto& uiXform = uiEntity.GetComponent<Component::Transform>(); //ui element transform in screen space

				if (uiEntity.HasComponent<Component::Text>()) {
					continue;
					/*
					auto& textComp = uiEntity.GetComponent<Component::Text>();

					if (std::shared_ptr<Systems::TextSystem> textSys =
						Systems::SystemManager::GetInstance().GetSystem<Systems::TextSystem>().lock()) {

						if (textComp.fontFamilyName == "None") { continue; }
						if (!IGE_ASSETMGR.IsGUIDValid<IGE::Assets::FontAsset>(textComp.textAsset)) {
							Debug::DebugLogger::GetInstance().LogWarning("[Text] Invalid Text Asset attached to Entity: "
								+ ECS::Entity{ entity }.GetTag());
							continue;
						}

						Systems::Font const& fontAsset{ IGE_ASSETMGR.GetAsset<IGE::Assets::FontAsset>(textComp.textAsset)->mFont };

						if (!textSys->IsValid(fontAsset)) {
							Debug::DebugLogger::GetInstance().LogWarning("[Text] Invalid Font attached to Entity: "
								+ ECS::Entity{ entity }.GetTag());
							continue;
						}

						textSys->CalculateNewLineIndices(textComp.textAsset, textComp.textContent,
							textComp.newLineIndices, textComp.newLineIndicesUpdatedFlag, textComp.alignment);

						shader->SetUniform("u_Font", 1);
						textSys->DrawTextFont(shader, fontAsset.mFilePathHash, textComp.textContent, uiXform.position.x,
							uiXform.position.y, textComp.scale, uiXform.rotation, textComp.color, textComp.newLineIndices, textComp.multiLineSpacingOffset);

						shader->Unuse();
					}
					*/
				}

				else if (uiEntity.HasComponent<Component::Image>()) {
					auto const& imageComp = uiEntity.GetComponent<Component::Image>();

					if (imageComp.textureAsset)
						Renderer::DrawSprite(uiXform.position, glm::vec2{ uiXform.scale}, uiXform.rotation, IGE_ASSETMGR.GetAsset<IGE::Assets::TextureAsset>(imageComp.textureAsset)->mTexture, imageComp.color);
					else
						Renderer::DrawQuad(uiXform.position, glm::vec2{ uiXform.scale }, uiXform.rotation, imageComp.color);
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

