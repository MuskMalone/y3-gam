#include <pch.h>
#include <Core/Systems/SystemManager/SystemManager.h>
#include <Core/LayerManager/LayerManager.h>
#include "RenderSystem.h"
#include "Core/EntityManager.h"
#include "Core/Entity.h"
#include "Renderer.h"
#include "Utils.h"
#include <Scenes/SceneManager.h>
#include "Events/Event.h"
#include "Events/EventManager.h"
#include <Input/InputManager.h>

namespace Graphics {
	CameraManager RenderSystem::mCameraManager;
	void RenderSystem::Init() {
		Renderer::Init();
	}

	std::vector<ECS::Entity> RenderSystem::RenderScene(CameraSpec const& cam) {
		PrepareFrame();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		std::vector<ECS::Entity> entityVector{ GetEntitiesToRender(cam) };

		std::shared_ptr<Texture> prevOutputTex{ nullptr };
		for (auto const& pass : Renderer::mRenderPasses) {
			if (prevOutputTex) {
				pass->SetInputTexture(prevOutputTex);
			}
			pass->Render(cam, entityVector);
			prevOutputTex = pass->GetOutputTexture();
		}

		if (!cam.isEditor)
			HandleGameViewInput();
		return entityVector;
	}

	std::vector<ECS::Entity> RenderSystem::GetEntitiesToRender(CameraSpec const& cam) {
		IGE::Assets::AssetManager& am{ IGE_ASSETMGR };
		BV::Frustum const& camFrustum{ cam.GetFrustum() };
		unsigned cullCount{};

		/* ========== Frustum Culling ========== */
		// TEMP FIX - Dont cull
		/*auto shouldRender = [](ECS::Entity entity) {
			return entity.IsActive();
		};*/
		auto shouldRender = [&am, &camFrustum, &cullCount](ECS::Entity entity) {
			if (!entity.IsActive()) { return false; }
			// lights and entities without mesh shouldnt be culled
			else if (entity.HasComponent<Component::Light>() || entity.HasComponent<Component::Bloom>() || !entity.HasComponent<Component::Mesh>()) {
				return true;
			}

			Component::Mesh& mesh{ entity.GetComponent<Component::Mesh>() };
			if (!mesh.meshSource) { return false; }

#ifdef _DEBUG
			try {
#endif
				bool const ret{ Utils::Culling::EntityInViewFrustum(camFrustum, entity.GetComponent<Component::Transform>(),
					am.GetAsset<IGE::Assets::ModelAsset>(mesh.meshSource)->mMeshSource) };
				if (!ret) { ++cullCount; }

				return ret;
#ifdef _DEBUG
			}
			catch (Debug::ExceptionBase&) {
				IGE_DBGLOGGER.LogError("Unable to get asset " + std::to_string((uint64_t)mesh.meshSource));
			}

			return true;
#endif
		};

		std::vector<ECS::Entity> entityVector{};
		// if editing prefab, pass in all active entities
		if (Scenes::SceneManager::GetInstance().GetSceneState() == Scenes::PREFAB_EDITOR) {
			auto entities{ ECS::EntityManager::GetInstance().GetAllEntities() };
			entityVector.reserve(entities.size());
			std::copy_if(std::make_move_iterator(entities.begin()),
				std::make_move_iterator(entities.end()), std::back_inserter(entityVector),
				shouldRender);
		}
		// else call on the layer system
		else {
			Layers::LayerManager& layerManager{ IGE_LAYERMGR };
			std::unordered_map<std::string, std::vector<ECS::Entity>> const& layerEntities{ layerManager.GetLayerEntities() };
			for (auto const& [layerName, entities] : layerEntities) {
				if (layerManager.IsLayerVisible(layerName)) {
					std::copy_if(entities.begin(), entities.end(), std::back_inserter(entityVector),
						shouldRender);
				}
			}
		}

		if (cam.isEditor) {
			mEditorCullCount = cullCount;
		}
		else {
			mGameCullCount = cullCount;
		}

		return entityVector;
	}

	void RenderSystem::RenderScene(CameraSpec const& cam, std::vector<ECS::Entity> const& entities) {
		PrepareFrame();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		std::shared_ptr<Texture> prevOutputTex{ nullptr };
		for (auto const& pass : Renderer::mRenderPasses) {
			if (prevOutputTex) {
				pass->SetInputTexture(prevOutputTex);
			}
			pass->Render(cam, entities);
			prevOutputTex = pass->GetOutputTexture();
		}
	}

	void RenderSystem::Release() {

	}

	void RenderSystem::PrepareFrame() {
		// Update material properties and check if any changes were made
		if (MaterialTable::UpdateMaterialPropsBuffer()) {
			// Only upload to the GPU if materials were updated
			MaterialTable::UploadMaterialProps();
		}
	}

	void RenderSystem::HandleGameViewInput() {
		static ECS::Entity prevHoveredEntity{};

		glm::vec2 mousePos = Input::InputManager::GetInstance().GetMousePos();
		const int entId = Renderer::PickEntity(mousePos, glm::vec2{}, glm::vec2{});
	
		ECS::Entity const hoveredEntity{ static_cast<ECS::Entity::EntityID>(entId) };

		if (hoveredEntity != prevHoveredEntity) {
			if (prevHoveredEntity) {
				QUEUE_EVENT(Events::EntityMouseExit, prevHoveredEntity);
			}
			if (hoveredEntity) {
				QUEUE_EVENT(Events::EntityMouseEnter, hoveredEntity);
			}
		}

		prevHoveredEntity = hoveredEntity;
		if (hoveredEntity) {
			if (Input::InputManager::GetInstance().IsKeyTriggered(IK_MOUSE_LEFT)) {
				QUEUE_EVENT(Events::EntityMouseDown, hoveredEntity);
			}
			if (Input::InputManager::GetInstance().IsKeyReleased(IK_MOUSE_LEFT)) {
				QUEUE_EVENT(Events::EntityMouseUp, hoveredEntity);
			}
		}
	}
}
