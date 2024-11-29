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
#include <Graphics/Camera/CameraManager.h>
#include <Graphics/Camera/CameraSpec.h>
#include <vector>

namespace ECS { class Entity; }

namespace Graphics {
	class EditorCamera;

	class RenderSystem {
	public:
		static void Init();
		static void Release();
		static std::vector<ECS::Entity> RenderScene(CameraSpec const& cam);
		static void RenderScene(CameraSpec const& cam, std::vector<ECS::Entity> const& entities);

		static void PrepareFrame();
		static void HandleGameViewInput();
		static void HandleUIInput(std::vector<ECS::Entity> const& uiEntities);
		static CameraManager mCameraManager; // Add CameraManager as a static member

		static uint32_t GetEditorCullCount() { return mEditorCullCount; }
		static uint32_t GetGameCullCount() { return mGameCullCount; }

	private:
		// can move into struct if needed
		static inline uint32_t mEditorCullCount = 0, mGameCullCount = 0;

		static std::vector<ECS::Entity> GetEntitiesToRender(CameraSpec const& cam);
	};
}