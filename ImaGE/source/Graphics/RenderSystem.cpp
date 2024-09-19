#include <pch.h>
#include "Core/Component/Components.h"
#include "RenderSystem.h"
#include "Core/EntityManager.h"
#include "Core/Entity.h"
#include "Renderer.h"
#include "Utils.h"

namespace Graphics {
	void RenderSystem::RenderScene(const EditorCamera& eCam) {

		auto& entManager = ECS::EntityManager::GetInstance();
		auto entityList = entManager.GetAllEntitiesWithComponents<Component::Transform, Component::Mesh>();

		Renderer::RenderSceneBegin(eCam.GetViewProjMatrix());

		for (ECS::Entity entity : entityList) {
			auto const& xfm = entity.GetComponent<Component::Transform>();
			auto const& mesh = entity.GetComponent<Component::Mesh>();

			Graphics::Renderer::SubmitMesh(mesh.mesh, xfm.worldPos, xfm.worldScale, {1.f,1.f,1.f,1.f}, {}); //@TODO change clr and rot 
		}
		Renderer::RenderSceneEnd();
		//Utils::RenderContext renderContext(eCam.GetViewProjMatrix());
	}
}