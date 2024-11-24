#include <pch.h>
#include "Events/EventManager.h"
#include <Core/Systems/SystemManager/SystemManager.h>
#include <Core/LayerManager/LayerManager.h>
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
	std::unique_ptr<Graphics::RenderSystem::EventHandler> Graphics::RenderSystem::mEventHandler = nullptr;

	void RenderSystem::Init() {
		Renderer::Init();

		mEventHandler = std::make_unique<EventHandler>();
	}

	void RenderSystem::Release() {

	}

	// Implement EventHandler constructor and destructor
	Graphics::RenderSystem::EventHandler::EventHandler() {
		// Subscribe to pointer events
		SUBSCRIBE_CLASS_FUNC(Events::EventType::POINTER_ENTER, &EventHandler::OnPointerEnter, this);
		SUBSCRIBE_CLASS_FUNC(Events::EventType::POINTER_EXIT, &EventHandler::OnPointerExit, this);
	}

	Graphics::RenderSystem::EventHandler::~EventHandler() {

	}
	 
	void RenderSystem::EventHandler::HandleUIInteractions(const std::vector<ECS::Entity>& uiEntities) {
	}

	EVENT_CALLBACK_DEF(RenderSystem::EventHandler, OnPointerEnter) {
		auto const& entity = CAST_TO_EVENT(Events::PointerEnterEvent)->mEntity;

		if (entity.HasComponent<Component::Script>()) {
			auto& script = entity.GetComponent<Component::Script>();
			
		}
	}

	EVENT_CALLBACK_DEF(RenderSystem::EventHandler, OnPointerExit) {

	}

	void RenderSystem::RenderScene(CameraSpec const& cam) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Frustum Culling should be here

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
			Layers::LayerManager& layerManager{ IGE_LAYERMGR };
			std::unordered_map<std::string, std::vector<ECS::Entity>> const& layerEntities{ layerManager.GetLayerEntities() };
			for (std::pair<std::string, std::vector<ECS::Entity>> const& mapPair : layerEntities) {
				if (layerManager.IsLayerVisible(mapPair.first)) {
#ifdef INSERT_ACTIVE
					layerManager.CopyValidEntities(entityVector, mapPair);
#else
					entityVector.insert(entityVector.end(), mapPair.second.begin(), mapPair.second.end());
#endif
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