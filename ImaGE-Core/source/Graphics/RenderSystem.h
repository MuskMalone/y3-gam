/*!*********************************************************************
\file   RenderSystem.h
\author k.choa\@digipen.edu
\date   22/09/2024
\brief  The RenderSystem class is responsible for initializing the renderer, managing scene rendering, and
		handling entity rendering in the editor environment. It utilizes the `Renderer` class to submit entities
		with meshes and materials for rendering, and it supports optimizations like frustum culling.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include "EditorCamera.h"
#include <Core/Systems/System.h>

namespace Graphics {
	class RenderSystem {

	public:
		static void Init();
		static void Release();
		static void RenderScene();
		static void RenderEditorScene(const EditorCamera& eCam);

	};
}