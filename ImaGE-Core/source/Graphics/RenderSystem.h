#pragma once
#include "EditorCamera.h"
namespace Graphics {
	class RenderSystem {

	public:
		static void Init();
		static void Release();
		static void RenderScene();
		static void RenderEditorScene(const EditorCamera& eCam);

	};
}