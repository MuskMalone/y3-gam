/*!*********************************************************************
\file   RenderSystem.h
\author k.choa\@digipen.edu
\date   22/09/2024
\brief  The RenderSystem class is responsible for initializing the
				renderer, managing scene rendering, and handling entity 
				rendering in the editor environment. It utilizes the `Renderer`
				class to submit entities with meshes and materials for rendering,
				and it supports optimizations like frustum culling.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include "CameraManager.h"
#include "CameraSpec.h"

namespace Graphics {
	class EditorCamera;

	class RenderSystem {

	public:
		static void Init();
		static void Release();
		static void RenderScene(CameraSpec const& cam);
		static CameraManager mCameraManager; // Add CameraManager as a static member

	private:
		class EventHandler {
		public:
			EventHandler();
			~EventHandler();

			void HandleUIInteractions(const std::vector<ECS::Entity>& uiEntities);
			EVENT_CALLBACK_DECL(OnPointerEnter);
			EVENT_CALLBACK_DECL(OnPointerExit);
		};
		static std::unique_ptr<EventHandler> mEventHandler;
	};
}