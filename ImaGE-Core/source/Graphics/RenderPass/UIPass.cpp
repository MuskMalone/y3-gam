#include "pch.h"
#include "UIPass.h"
#include "Graphics/Renderer.h"
#include "Core/Components/Canvas.h"
#include "Core/Components/Transform.h"
#include "Core/Components/Text.h"
#include "Core/Components/Image.h"
#include <Physics/PhysicsSystem.h>
#include "GeomPass.h"

namespace Graphics {
	UIPass::UIPass(const RenderPassSpec& spec) : RenderPass{ spec } {

	}

	void UIPass::Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities){

		Begin();
		auto const& shader{ mSpec.pipeline->GetShader() };

		glm::mat4 viewProj{};
		viewProj = cam.isEditor ? cam.viewProjMatrix : Renderer::mUICamera.GetViewProjMatrix();

		// @TODO: TEMP, TO MERGE WITH XAVIER
		//shader->Use();
		//shader->SetUniform("u_ViewProjMtx", viewProj);

		Renderer::RenderSceneBegin(viewProj, cam);

		if (cam.isEditor) {
			for (auto const& light : entities) {
				if (!light.HasComponent<Component::Transform>()) continue;
				if (!light.HasComponent<Component::Light>()) continue;
				auto const& xform = ECS::Entity{ light }.GetComponent<Component::Transform>();
				auto const& lightComp = ECS::Entity{ light }.GetComponent<Component::Light>();
				Renderer::DrawLightGizmo(lightComp, xform, cam, ECS::Entity{ light }.GetEntityID());
			}
			//auto const& cameras = ecsMan.GetAllEntitiesWithComponents<Component::Camera>();
			//for (auto const& camera : cameras) {
			//    if (!ECS::Entity{ camera }.IsActive()) continue;
			//    auto const& camComp = ECS::Entity{ camera }.GetComponent<Component::Camera>();
			//    auto const& xform = ECS::Entity{ camera }.GetComponent<Component::Transform>();
			//    Renderer::DrawSprite(xform.worldPos, glm::vec2{ xform.worldScale }, xform.worldRot, IGE_ASSETMGR.GetAsset<IGE::Assets::TextureAsset>(Renderer::mIcons[2])->mTexture, Color::COLOR_WHITE, ECS::Entity { camera }.GetEntityID(), true, cam);
			//}

		}

		//temp physics debug hack
		if (cam.isEditor) {
			IGE::Physics::PhysicsSystem::GetInstance()->Debug();
			Graphics::ParticleManager::GetInstance().Debug();
		}
		if (cam.isEditor) {
			for (auto const& camera : entities ) {
				if (!camera.HasComponent<Component::Camera>()) continue;
				if (!camera.IsActive()) continue;
				auto const& camComp = camera.GetComponent<Component::Camera>();
				auto const& xform = camera.GetComponent<Component::Transform>();
				Renderer::DrawCameraFrustrum(camComp, Color::COLOR_CYAN);
			}
		}

		for (ECS::Entity entity : entities) {

			if (!entity.HasComponent<Component::Canvas>()) { continue; } //if not canvas skip
			auto const& canvas{ entity.GetComponent<Component::Canvas>() };
			if (!entity.IsActive()) continue;
			
			//canvas found

			ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
			// if no children, early exit
			if (!entityMan.HasChild(entity)) {
				//GE_DBGLOGGER.LogWarning("Canvas entity requires a child");
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
			else {
				Graphics::Renderer::HandleUIInput(children);
			}

			for (ECS::Entity uiEntity : children) {
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

				if (uiEntity.HasComponent<Component::Image>()) {
					auto const& imageComp = uiEntity.GetComponent<Component::Image>();

					if (imageComp.textureAsset)
						Renderer::DrawSprite(uiXform.worldPos, glm::vec2{ uiXform.worldScale}, uiXform.worldRot, IGE_ASSETMGR.GetAsset<IGE::Assets::TextureAsset>(imageComp.textureAsset)->mTexture, imageComp.color);
					else
						Renderer::DrawQuad(uiXform.worldPos, glm::vec2{ uiXform.worldScale }, uiXform.worldRot, imageComp.color);
						
				}

				/*
				else {
					Renderer::DrawRect(uiXform.worldPos, glm::vec2{ uiXform.worldScale }, uiXform.worldRot, Color::COLOR_WHITE);
				}
				*/

				//Graphics::Renderer::SubmitInstance(Renderer::GetQuadMeshSource(), uiXform.worldMtx, Color::COLOR_WHITE, uiEntity.GetEntityID());
			}
		}

		Renderer::RenderSceneEnd();

		if (cam.isEditor) {
			auto const& highlightShader{ ShaderLibrary::Get("Highlight") };
			highlightShader->Use();
			auto const& fb{ Renderer::GetPass<GeomPass>()->GetTargetFramebuffer() };
			auto const& fbSpec{ fb->GetFramebufferSpec() };
			auto const& redTex{ Renderer::GetPass<GeomPass>()->GetEntityTexture() };

			highlightShader->SetUniform("u_ScreenTex", mInputTexture, 30);
			highlightShader->SetUniform("u_EntityTex", redTex, 31);
			highlightShader->SetUniform("u_TexelSize", glm::vec2{1.f/fbSpec.width, 1.f/fbSpec.height});
			int ent = static_cast<int>(Renderer::GetHighlightedEntity().GetEntityID());
			highlightShader->SetUniform("u_SelectedEntity", ent);
			

			Renderer::RenderFullscreenTexture();
		}

		// @TODO: TEMP, TO MERGE WITH XAVIER
		if (std::shared_ptr<Systems::TextSystem> textSys =
			Systems::SystemManager::GetInstance().GetSystem<Systems::TextSystem>().lock()) {
			textSys->RenderTextForAllEntities(viewProj, entities);
		}

		//Renderer::RenderInstances();

		
		//================== TRANSITION PART =========================================
		if (!cam.isEditor) {  // Apply transition only in gameplay, not the editor
			for (ECS::Entity const& entity : entities) {
				if (!entity.HasComponent<Component::Canvas>()) continue; // Skip if no Canvas

				auto const& canvas = entity.GetComponent<Component::Canvas>();
				if (!canvas.hasTransition) continue; // Skip if transition is not active

				auto const& transitionShader = ShaderLibrary::Get("Transition");
				transitionShader->Use();

				// Pass transition progress (0.0 = black, 1.0 = fully visible)
				transitionShader->SetUniform("u_TransitionProgress", canvas.transitionProgress);

				// Pass fade color (default to black)
				transitionShader->SetUniform("u_FadeColor", canvas.fadeColor);

				// Check if we're fading out or in
				transitionShader->SetUniform("u_FadeOut", canvas.fadingOut);

				transitionShader->SetUniform("u_TransitionType", static_cast<int>(canvas.transitionType));


				// Render the final scene with the transition effect applied
				Renderer::RenderFullscreenTexture(true);
			}
		}


		End();
	}
}

