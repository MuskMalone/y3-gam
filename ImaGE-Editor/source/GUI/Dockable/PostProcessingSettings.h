#pragma once
#include <GUI/GUIWindow.h>

namespace Graphics { class PostProcessingManager; }

namespace GUI {
	class PostProcessingSettings : public GUIWindow {
	public:
		PostProcessingSettings(const char* name);
		void Run() override;

	private:
		void ShaderDragDropEvent();

		Graphics::PostProcessingManager& mPostProcessingMgr;
	};
}