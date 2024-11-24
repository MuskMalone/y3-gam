#include "pch.h"
#include "UIPass.h"
#include "Graphics/Renderer.h"
#include "Core/Components/Canvas.h"
#include "Core/Components/Transform.h"
#include "Core/Components/Text.h"
#include "Core/Components/Image.h"
#include <Physics/PhysicsSystem.h>
#include <Input/InputManager.h>

namespace Graphics {
	UIPass::UIPass(const RenderPassSpec& spec) : RenderPass{ spec } {

	}

	void UIPass::Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities){

		Begin();
		auto const& shader{ mSpec.pipeline->GetShader() };

		glm::mat4 viewProj{};
		viewProj = cam.isEditor ? cam.viewProjMatrix : Renderer::mUICamera.GetViewProjMatrix();

		auto const& fb = mSpec.pipeline->GetSpec().targetFramebuffer;
		float viewportHeight = fb->GetFramebufferSpec().height;
		float viewportWidth = fb->GetFramebufferSpec().width;
		auto const& inputMan = Input::InputManager::GetInstance();

		// @TODO: TEMP, TO MERGE WITH XAVIER
		//shader->Use();
		//shader->SetUniform("u_ViewProjMtx", viewProj);

		Renderer::RenderSceneBegin(viewProj);

		//temp physics debug hack
		if (cam.isEditor) 
			IGE::Physics::PhysicsSystem::GetInstance()->Debug();

		auto& ecsMan{ ECS::EntityManager::GetInstance() };
		if (cam.isEditor) {
			auto const& cameras = ecsMan.GetAllEntitiesWithComponents<Component::Camera>();
			for (auto const& camera : cameras) {
				if (!ECS::Entity{ camera }.IsActive()) continue;
				auto const& camComp = ECS::Entity{ camera }.GetComponent<Component::Camera>();
				auto const& xform = ECS::Entity{ camera }.GetComponent<Component::Transform>();
				Renderer::DrawCameraFrustrum(camComp, Color::COLOR_CYAN);
			}
			//auto const& lights = ecsMan.GetAllEntitiesWithComponents<Component::Light, Component::Transform>();
			//for (auto const& light : lights) {
			//	if (!ECS::Entity{ light }.IsActive()) continue;
			//	auto const& xform = ECS::Entity{ light }.GetComponent<Component::Transform>();
			//	auto const& lightComp = ECS::Entity{ light }.GetComponent<Component::Light>();
			//	Renderer::DrawLightGizmo(lightComp, xform);
			//}
		}

		for (ECS::Entity const& entity : entities) {

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

			std::vector<ECS::Entity> children{ entityMan.GetChildEntityRecursively(entity) };
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

				/*
				if (uiEntity.HasComponent<Component::Text>()) {
					continue;


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
				}
				*/

				// Handle interactive components
				if (uiEntity.HasComponent<Component::Interactive>()) {
					auto& interactiveComp = uiEntity.GetComponent<Component::Interactive>();
					// Get mouse position in screen coordinates
					glm::vec2 mousePos = inputMan.GetMousePos(); // x and y in pixels
					// Convert mouse position to world coordinates (assuming orthographic projection)
					float worldX = bounds.x + (mousePos.x / viewportWidth) * (bounds.y - bounds.x);
					float worldY = bounds.w - (mousePos.y / viewportHeight) * (bounds.w - bounds.z); // Flip y-axis
					glm::vec2 mouseWorldPos = glm::vec2(worldX, worldY);
					// Compute the bounds of the UI element
					glm::vec2 position = uiXform.worldPos; // Center position
					glm::vec2 size = uiXform.worldScale;   // Width and height

					// Compute bounds: left, top, width, height
					glm::vec4 uiBounds{};
					uiBounds.x = position.x - size.x * 0.5f; // left
					uiBounds.y = position.y - size.y * 0.5f; // top
					uiBounds.z = size.x;                     // width
					uiBounds.w = size.y;                     // height

					// Call isMouseOver
					bool isHovered = interactiveComp.IsMouseOver(mouseWorldPos, uiBounds);

					// Update the isHovered state
					interactiveComp.isHovered = isHovered;

				}

				if (uiEntity.HasComponent<Component::Image>()) {
					auto const& imageComp = uiEntity.GetComponent<Component::Image>();

					if (imageComp.textureAsset)
						Renderer::DrawSprite(uiXform.worldPos, glm::vec2{ uiXform.worldScale}, uiXform.worldRot, IGE_ASSETMGR.GetAsset<IGE::Assets::TextureAsset>(imageComp.textureAsset)->mTexture, imageComp.color);
					else
						Renderer::DrawQuad(uiXform.worldPos, glm::vec2{ uiXform.worldScale }, uiXform.worldRot, imageComp.color);
						
				}

				else {
					//Renderer::DrawRect(uiXform.worldPos, glm::vec2{ uiXform.worldScale }, uiXform.worldRot, Color::COLOR_WHITE);
				}
				

				//Graphics::Renderer::SubmitInstance(Renderer::GetQuadMeshSource(), uiXform.worldMtx, Color::COLOR_WHITE, uiEntity.GetEntityID());
			}
		}

		Renderer::RenderSceneEnd();

		// @TODO: TEMP, TO MERGE WITH XAVIER
		if (std::shared_ptr<Systems::TextSystem> textSys =
			Systems::SystemManager::GetInstance().GetSystem<Systems::TextSystem>().lock()) {
			textSys->RenderTextForAllEntities(viewProj, entities);
		}

		//Renderer::RenderInstances();

		End();
	}
}

