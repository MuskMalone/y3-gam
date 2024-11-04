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
	}

	void RenderSystem::Release() {

	}

	void RenderSystem::RenderScene(CameraSpec const& cam) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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