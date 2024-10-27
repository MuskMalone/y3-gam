#include <pch.h>
#include <Core/Components/Components.h>
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

		auto& entManager = ECS::EntityManager::GetInstance();
		auto entityList = entManager.GetAllEntitiesWithComponents<Component::Transform, Component::Mesh>();

		//Frustum Culling should be here
		
		// Convert to a vector
		std::vector<ECS::Entity> entityVector;
		for (auto entity : entityList) {
			entityVector.push_back(entity);
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








