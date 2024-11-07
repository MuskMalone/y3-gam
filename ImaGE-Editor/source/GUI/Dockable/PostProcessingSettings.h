#pragma once

#include <GUI/GUIWindow.h>

namespace GUI {
	class PostProcessingSettings : public GUIWindow {
	public:
		PostProcessingSettings(const char* name);
		void Run() override;

	};
}