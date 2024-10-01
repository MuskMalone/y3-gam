#include "pch.h"
#ifndef IMGUI_DISABLE
#include "Console.h"
#include <ImGui/imgui.h>

namespace GUI {

  Console::Console(const char* name) : GUIWindow(name),
    mTerminal{ mSettings, name }, mSettings{} {

		Debug::DebugLogger& logger{ Debug::DebugLogger::GetInstance() };
		logger.AddDest(mTerminal.get_terminal_helper());
		logger.AddFileDest(mTerminal.get_terminal_helper());

		auto& theme = mTerminal.theme();
		theme.log_level_colors = {
			// Same colours as ImTerm/utils.hpp
			ImTerm::theme::constexpr_color{0.078f, 0.117f, 0.764f, 1.f}, // log_level::trace
			ImTerm::theme::constexpr_color{0.100f, 0.100f, 0.100f, 1.f}, // log_level::debug
			ImTerm::theme::constexpr_color{0.301f, 0.529f, 0.000f, 1.f}, // log_level::info
			ImTerm::theme::constexpr_color{0.784f, 0.431f, 0.058f, 1.f}, // log_level::warning
			ImTerm::theme::constexpr_color{0.901f, 0.117f, 0.117f, 1.f}, // log_level::error
			ImTerm::theme::constexpr_color{0.901f, 0.117f, 0.117f, 1.f}, // log_level::critical
		};

		SetMaxLength(100);
  }
  
  void Console::Run() {
    mTerminal.show();
  }

	void Console::SetMaxLength(size_t length) {
		mTerminal.set_max_log_len(length);
	}
}

#endif  // IMGUI_DISABLE
