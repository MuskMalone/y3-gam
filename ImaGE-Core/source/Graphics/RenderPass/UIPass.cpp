#include "pch.h"
#include "UIPass.h"
#include "Graphics/Renderer.h"
#include "Core/Components/Canvas.h"
#include "Core/Components/Transform.h"
#include "Core/Components/Text.h"
#include "Core/Components/Image.h"
#include <Physics/PhysicsSystem.h>

namespace Graphics {
	UIPass::UIPass(const RenderPassSpec& spec) : RenderPass{ spec } {

	}

	void UIPass::Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities){

		Begin();
		auto const& shader{ mSpec.pipeline->GetShader() };

		glm::mat4 viewProj{};
		viewProj = cam.isEditor ? cam.viewProjMatrix : Renderer::mUICamera.GetViewProjMatrix();

		// @TODO: TEMP, TO MERGE WITH XAVIER
		shader->Use();
		shader->SetUniform("u_ViewProjMtx", viewProj);

		Renderer::RenderSceneBegin(viewProj);

		//temp physics debug hack
		if (cam.isEditor) 
			IGE::Physics::PhysicsSystem::GetInstance()->Debug();


		for (ECS::Entity const& entity : entities) {

			if (cam.isEditor) {
				auto const& cameras = ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::Camera>();
				for (auto const& camera : cameras) {
					auto const& camComp = ECS::Entity{ camera }.GetComponent<Component::Camera>();
					if(!ECS::Entity{ camera }.IsActive())continue;
					Renderer::DrawCameraFrustrum(camComp, Color::COLOR_CYAN);
				}
			}

			if (!entity.HasComponent<Component::Canvas>()) { continue; } //if not canvas skip
			auto const& canvas{ entity.GetComponent<Component::Canvas>() };
			if (!entity.IsActive()) continue;
			
			//canvas found

			ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
			// if no children, early exit
			if (!entityMan.HasChild(entity)) {
				IGE_DBGLOGGER.LogWarning("Canvas entity requires a child");
				Renderer::RenderSceneEnd();
				End();
				return;
			}

			auto children{ entityMan.GetChildEntity(entity) }; //vector of UI element entity
			auto const& xform = entity.GetComponent<Component::Transform>(); //canvas xform
			
			// Sort children by Z index (pos.z of Transform component)
			std::sort(children.begin(), children.end(), [](ECS::Entity const& a, ECS::Entity const& b) {
				auto& xformA = a.GetComponent<Component::Transform>();
				auto& xformB = b.GetComponent<Component::Transform>();
				return xformA.position.z < xformB.position.z; // Ascending order
				});

			// Calculate scale for the canvas based on ortho cam
			glm::vec4 bounds = Renderer::mUICamera.GetOrthographicBounds();
			float orthoWidth = bounds.y - bounds.x; // right - left
			float orthoHeight = bounds.w - bounds.z; // top - bottom
			//float orthoWidth = 2.0f * cam.aspectRatio * UI_SCALING_FACTOR<float>;
			//float orthoHeight = 2.0f * cam.aspectRatio * UI_SCALING_FACTOR<float>;
			glm::vec2 canvasScale = glm::vec2{ orthoWidth, orthoHeight };

			if (cam.isEditor) {
				Graphics::Renderer::DrawRect(xform.position, canvasScale, xform.rotation, Color::COLOR_WHITE); //canvas drawn only in editor
			}

			for (ECS::Entity& uiEntity : children) {
				auto& uiXform = uiEntity.GetComponent<Component::Transform>(); //ui element transform in screen space

				// Since you are iterating through the child entities, you have to check this again
				if (!uiEntity.IsActive()) {
					continue;
				}

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
						Renderer::DrawSprite(uiXform.worldPos, glm::vec2{ uiXform.worldScale}, uiXform.worldRot, IGE_ASSETMGR.GetAsset<IGE::Assets::TextureAsset>(imageComp.textureAsset)->mTexture, imageComp.color);
					else
						Renderer::DrawQuad(uiXform.worldPos, glm::vec2{ uiXform.worldScale }, uiXform.worldRot, imageComp.color);
						
				}

				else {
					Renderer::DrawRect(uiXform.worldPos, glm::vec2{ uiXform.worldScale }, uiXform.worldRot, Color::COLOR_WHITE);
				}
				

				//Graphics::Renderer::SubmitInstance(Renderer::GetQuadMeshSource(), uiXform.worldMtx, Color::COLOR_WHITE, uiEntity.GetEntityID());
			}
		}

		Renderer::RenderSceneEnd();

		if (std::shared_ptr<Systems::TextSystem> textSys =
			Systems::SystemManager::GetInstance().GetSystem<Systems::TextSystem>().lock()) {
			textSys->RenderTextForAllEntities(viewProj, entities);
		}

		//Renderer::RenderInstances();

		End();
	}
}

