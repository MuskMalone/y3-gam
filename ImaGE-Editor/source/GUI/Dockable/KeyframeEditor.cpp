#include <pch.h>
#include "KeyframeEditor.h"
#include <ImGui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>

namespace GUI {
  KeyframeEditor::KeyframeEditor(const char* windowName) : GUIWindow(windowName) {

  }

  void KeyframeEditor::Run() {
    ImGui::Begin(mWindowName.c_str());



    ImGui::End();
  }


} // namespace GUI
