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

	void RenderSystem::Init() {
		Renderer::Init();
	}

	void RenderSystem::Release() {

	}

	void RenderSystem::RenderScene() {

	}

	void RenderSystem::RenderEditorScene(const EditorCamera& eCam) {
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
			pass->Render(eCam, entityVector);
		}

		//Renderer::mGeomPass->Begin();
		//{//Render Start
		//	Utils::RenderContext renderContext(eCam.GetViewProjMatrix());

		//	for (ECS::Entity entity : entityList) {
		//		auto const& xfm = entity.GetComponent<Component::Transform>();
		//		auto const& mesh = entity.GetComponent<Component::Mesh>();
		//		if (mesh.mesh == nullptr) continue;

		//		
		//		Graphics::Renderer::SubmitMesh(mesh.mesh, xfm.worldPos, xfm.worldScale, { 1.f,1.f,1.f,1.f }, {}); //@TODO change clr and rot 
		//		// Assuming xfm.worldPos is a glm::vec3 that contains the position in world space
		//	}

		//} // Render End

		//Renderer::mGeomPass->End();

	}

}