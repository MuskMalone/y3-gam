#include <pch.h>
#include <Core/Components/Components.h>
#include <Core/Systems/LayerSystem/LayerSystem.h>
#include "RenderSystem.h"
#include "Core/EntityManager.h"
#include "Core/Entity.h"
#include "Renderer.h"
#include "Utils.h"
#include "EditorCamera.h"

namespace Graphics {
	CameraManager RenderSystem::mCameraManager;
	void RenderSystem::Init() {
		Renderer::Init();

		//ECS::Entity mainCameraEntity = ECS::EntityManager::GetInstance().CreateEntity();
		//auto & camCom = mainCameraEntity.EmplaceComponent<Component::Camera>(); // Add your Camera component
		//camCom.position = glm::vec3(0.0f, 5.0f, 20.0f);
		//camCom.yaw = -90.f;
		//camCom.pitch = -30.f;
		//
		//// Optionally, add a tag or mark it as the main camera
		//mainCameraEntity.SetTag("MainCamera"); // Assuming you have a SetTag method
		//Graphics::RenderSystem::mCameraManager.AddCamera(mainCameraEntity);
		//Graphics::RenderSystem::mCameraManager.SetActiveCamera(0);

	}

	void RenderSystem::Release() {

	}

	void RenderSystem::RenderScene(CameraSpec const& cam) {
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		//Frustum Culling should be here

		std::vector<ECS::Entity> entityVector{};
		if (std::shared_ptr<Systems::LayerSystem> layerSys =
			Systems::SystemManager::GetInstance().GetSystem<Systems::LayerSystem>().lock()) {
			std::unordered_map<std::string, std::vector<ECS::Entity>> const& layerEntities{ layerSys->GetLayerEntities() };

			for (std::pair<std::string, std::vector<ECS::Entity>> mapPair : layerEntities) {
				if (layerSys->IsLayerVisible(mapPair.first)) {
					entityVector.insert(entityVector.end(), mapPair.second.begin(), mapPair.second.end());
				}
			}
		}

		for (auto const& pass : Renderer::mRenderPasses) {
			pass->Render(cam, entityVector);
		}

	}

}