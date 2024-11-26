#include <pch.h>
#include <Core/Systems/SystemManager/SystemManager.h>
#include <Core/LayerManager/LayerManager.h>
#include "RenderSystem.h"
#include "Core/EntityManager.h"
#include "Core/Entity.h"
#include "Renderer.h"
#include "Utils.h"
#include <Graphics/Camera/EditorCamera.h>
#include <Scenes/SceneManager.h>
#include "Events/Event.h"
#include "Events/EventManager.h"
#include <Input/InputManager.h>
#include <BoundingVolumes/IntersectionTests.h>

namespace {
	bool EntityInViewFrustum(BV::Frustum const& frustum, Component::Transform const& transform, Graphics::MeshSource const* meshSource);
}

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

		//if(!cam.isEditor)
		//	HandleGameViewInput();
		return entityVector;
	}

	std::vector<ECS::Entity> RenderSystem::GetEntitiesToRender(CameraSpec const& cam) {
		IGE::Assets::AssetManager& am{ IGE_ASSETMGR };
		BV::Frustum const& camFrustum{ cam.GetFrustum() };
		unsigned cullCount{};

		/* ========== Frustum Culling ========== */
		auto shouldRender = [&am, &camFrustum, &cullCount](ECS::Entity entity) {
			if (!entity.IsActive()) { return false; }
			// directional lights shouldnt be culled
			else if (entity.HasComponent<Component::Light>()
				&& entity.GetComponent<Component::Light>().type == Component::DIRECTIONAL) {
				return true;
			}

			// for objects without mesh, use transform as the bounding vol
			Graphics::MeshSource const* meshPtr{ nullptr };
			if (entity.HasComponent<Component::Mesh>()) {
				Component::Mesh& mesh{ entity.GetComponent<Component::Mesh>() };
				if (mesh.meshSource) {
					meshPtr = &am.GetAsset<IGE::Assets::ModelAsset>(mesh.meshSource)->mMeshSource;
				}
			};

			bool const ret{ EntityInViewFrustum(camFrustum, entity.GetComponent<Component::Transform>(), meshPtr) };
			if (!ret) { ++cullCount; }

			return ret;
		};

		std::vector<ECS::Entity> entityVector{};
		// if editing prefab, pass in all active entities
		if (Scenes::SceneManager::GetInstance().GetSceneState() == Scenes::PREFAB_EDITOR) {
			auto const& entities{ ECS::EntityManager::GetInstance().GetAllEntities() };
			entityVector.reserve(entities.size());
			std::copy_if(entities.begin(), entities.end(), std::back_inserter(entityVector),
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
		static ECS::Entity prevHoveredEntity;

		glm::vec2 mousePos = Input::InputManager::GetInstance().GetMousePos();
		ECS::Entity hoveredEntity = Renderer::PickEntity(mousePos);

		if (hoveredEntity != prevHoveredEntity) {
			if (IGE_ENTITYMGR.IsValidEntity(prevHoveredEntity)) {
				QUEUE_EVENT(Events::EntityMouseExit, prevHoveredEntity);
			}
			if (IGE_ENTITYMGR.IsValidEntity(hoveredEntity)) {
				QUEUE_EVENT(Events::EntityMouseEnter, prevHoveredEntity);
			}
		}

		prevHoveredEntity = hoveredEntity;
		if (IGE_ENTITYMGR.IsValidEntity(hoveredEntity)) {
			if (Input::InputManager::GetInstance().IsKeyTriggered(IK_MOUSE_LEFT)) {
				QUEUE_EVENT(Events::EntityMouseDown, hoveredEntity);
			}
			if (Input::InputManager::GetInstance().IsKeyReleased(IK_MOUSE_LEFT)) {
				QUEUE_EVENT(Events::EntityMouseUp, hoveredEntity);
			}
		}

	}
}

namespace {
	bool EntityInViewFrustum(BV::Frustum const& frustum, Component::Transform const& transform, Graphics::MeshSource const* meshSource) {
		BV::AABB aabb{ transform.worldPos, transform.worldScale };

		// if mesh exists, apply scale to halfExtents of mesh
		if (meshSource) {
			aabb.halfExtents *= meshSource->GetBoundingBox().halfExtents;
		}
		// else halve the worldScale
		else {
			aabb.halfExtents *= 0.5f;
		}

		return BV::FrustumAABBIntersection(frustum, aabb);
	}
}
