#include <pch.h>
#include "PopupHelper.h"
#include <Events/EventManager.h>

namespace GUI {
  static bool sOpenPopup;
  static std::string sCurrentPopup;

  PopupHelper::PopupHelper(const char* windowName) : GUIWindow(windowName) {
    mPopupFunctions.emplace_back(std::bind(&PopupHelper::GUIDRemapPopup, this));
  }

  void PopupHelper::Run() {
    if (sOpenPopup) {
      ImGui::OpenPopup(sCurrentPopup.c_str());
      sOpenPopup = false;
    }

    for (PopupFn fn : mPopupFunctions) {
      fn();
    }
  }

  EVENT_CALLBACK_DEF(PopupHelper, TriggerGUIDRemap) {
    sCurrentPopup = sGUIDPopupTitle;
    sOpenPopup = true;
  }

  void PopupHelper::GUIDRemapPopup() {
    bool resolved{ false };
    while (!resolved) {
      ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
      if (ImGui::BeginPopupModal(sGUIDPopupTitle, NULL, ImGuiWindowFlags_AlwaysAutoResize)) {


        ImGui::EndPopup();
      }
    }
  }

} // namespace GUI
