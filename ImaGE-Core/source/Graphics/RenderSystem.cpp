#include <pch.h>
#include <Core/Systems/SystemManager/SystemManager.h>
#include <Core/Systems/LayerSystem/LayerSystem.h>
#include "RenderSystem.h"
#include "Core/EntityManager.h"
#include "Core/Entity.h"
#include "Renderer.h"
#include "Utils.h"
#include "EditorCamera.h"
#include <Scenes/SceneManager.h>

#define INSERT_ACTIVE

namespace Graphics {
	CameraManager RenderSystem::mCameraManager;
	void RenderSystem::Init() {
		Renderer::Init(); 

		ECS::Entity mainCameraEntity = ECS::EntityManager::GetInstance().CreateEntity();
		auto & camCom = mainCameraEntity.EmplaceComponent<Component::Camera>(); // Add your Camera component
		camCom.position = glm::vec3(0.0f, 5.0f, 20.0f);
		camCom.yaw = -90.f;
		camCom.pitch = -30.f;
		
		// Optionally, add a tag or mark it as the main camera
		mainCameraEntity.SetTag("MainCamera"); // Assuming you have a SetTag method
		Graphics::RenderSystem::mCameraManager.AddCamera(mainCameraEntity);
		Graphics::RenderSystem::mCameraManager.SetActiveCamera(0);

	}

	void RenderSystem::Release() {

	}

	void RenderSystem::RenderScene(CameraSpec const& cam) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Frustum Culling should be here

		// waiting for ernest reply before i revert the commented the changes
		std::vector<ECS::Entity> entityVector{};
		// if editing prefab, pass in all active entities
		if (Scenes::SceneManager::GetInstance().GetSceneState() == Scenes::PREFAB_EDITOR) {
			auto const& entities{ ECS::EntityManager::GetInstance().GetAllEntities() };
			entityVector.reserve(entities.size());
#ifdef INSERT_ACTIVE
			std::copy_if(entities.begin(), entities.end(), std::back_inserter(entityVector),
				[](ECS::Entity const& e) { return e.GetComponent<Component::Tag>().isActive; });
#else
			entityVector.insert(entityVector.end(), entities.begin(), entities.end());
#endif
		}
		// else call on the layer system
		else {
			if (std::shared_ptr<Systems::LayerSystem> layerSys =
				Systems::SystemManager::GetInstance().GetSystem<Systems::LayerSystem>().lock()) {
				std::unordered_map<std::string, std::vector<ECS::Entity>> const& layerEntities{ layerSys->GetLayerEntities() };
				for (std::pair<std::string, std::vector<ECS::Entity>> mapPair : layerEntities) {
					if (layerSys->IsLayerVisible(mapPair.first)) {
#ifdef INSERT_ACTIVE
						// assuming majority of entities in a layer will be active, so .size is a decent estimate
						entityVector.reserve(entityVector.size() + mapPair.second.size());
						// insert all active entities
						std::copy_if(mapPair.second.begin(), mapPair.second.end(), std::back_inserter(entityVector),
							[](ECS::Entity const& e) { return e.GetComponent<Component::Tag>().isActive; });
#else
						entityVector.insert(entityVector.end(), mapPair.second.begin(), mapPair.second.end());
#endif
					}
				}
			}
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

}