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
#include <BoundingVolumes/IntersectionTests.h>

namespace {
	bool EntityInViewFrustum(BV::Frustum const& frustum, Component::Transform const& transform, Graphics::MeshSource const* meshSource);
}

namespace Graphics {
	CameraManager RenderSystem::mCameraManager;
	void RenderSystem::Init() {
		Renderer::Init();
	}

	void RenderSystem::RenderScene(CameraSpec const& cam) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Frustum Culling should be here
		IGE::Assets::AssetManager& am{ IGE_ASSETMGR };
		BV::Frustum const& camFrustum{ cam.GetFrustum() };
		
		unsigned cullCount{};
		auto shouldRender = [&am, &camFrustum, &cullCount](ECS::Entity entity) {
			if (!entity.IsActive()) { return false; }
			// directional lights shouldnt be culled
			else if (entity.HasComponent<Component::Light>()
				&& entity.GetComponent<Component::Light>().type == Component::DIRECTIONAL) {
				return true;
			}

			// for objects without mesh, use transform as the bounding vol
			Graphics::MeshSource const* meshPtr{ entity.HasComponent<Component::Mesh>() ?
				&am.GetAsset<IGE::Assets::ModelAsset>(entity.GetComponent<Component::Mesh>().meshSource)->mMeshSource : nullptr };

			bool const ret{ EntityInViewFrustum(camFrustum, entity.GetComponent<Component::Transform>(), meshPtr) };
			if (!ret) {
				++cullCount;
			}
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
			for (auto const&[layerName, entities] : layerEntities) {
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

		std::shared_ptr<Texture> prevOutputTex{ nullptr };
		for (auto const& pass : Renderer::mRenderPasses) {
			if (prevOutputTex) {
				pass->SetInputTexture(prevOutputTex);
			}
			pass->Render(cam, entityVector);
			prevOutputTex = pass->GetOutputTexture();
		}

	}

	void RenderSystem::Release() {

	}
}

namespace {
	bool EntityInViewFrustum(BV::Frustum const& frustum, Component::Transform const& transform, Graphics::MeshSource const* meshSource) {
		BV::BoundingSphere bs{ meshSource ?
			meshSource->GetBoundingVol() : BV::BoundingSphere() };
		bs.center = transform.worldPos;
		bs.radius *= std::max(transform.worldScale.x, std::max(transform.worldScale.y, transform.worldScale.z));

		if (!meshSource) {
			bs.radius *= 0.5f;
		}

		return BV::FrustumSphereIntersection(frustum, bs);
	}
}
