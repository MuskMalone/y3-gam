#include <pch.h>
#include "PopupHelper.h"
#include <Events/EventManager.h>
#include <Events/AssetEvents.h>
#include <ImGui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <GUI/Helpers/AssetHelpers.h>

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

namespace {
  static bool sOpenPopup;
  static std::string sCurrentPopup;
}

namespace GUI {
  PopupHelper::PopupHelper(const char* windowName) : GUIWindow(windowName) {
    mPopupFunctions.emplace_back(std::bind(&PopupHelper::GUIDRemapPopup, this));

    SUBSCRIBE_CLASS_FUNC(Events::EventType::INVALID_GUID, &PopupHelper::OnGUIDRemap, this);
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

  void EndDaFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // for floating windows feature
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      GLFWwindow* backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }
  }

  EVENT_CALLBACK_DEF(PopupHelper, OnGUIDRemap) {
    auto const& remapEvent{ CAST_TO_EVENT(Events::GUIDInvalidated) };
    ImGui::OpenPopup(sGUIDPopupTitle);
    EndDaFrame();
    bool resolved{ false };
    std::string newPath{};

    while (!resolved) {
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();
      ImGuizmo::BeginFrame();
      ImGui::DockSpaceOverViewport();

      ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
      if (ImGui::BeginPopupModal(sGUIDPopupTitle, NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("We were unable to locate ");
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f));
        ImGui::Text((remapEvent->mFileType + " (").c_str());
        ImGui::SameLine();
        ImGui::Text((std::to_string(static_cast<uint64_t>(remapEvent->mGUID)) + ")").c_str());
        ImGui::PopStyleColor();

        ImGui::NewLine();
        ImGui::Text("Affected Entity:");
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f));
        ImGui::Text((remapEvent->mEntityName + "(").c_str());
        ImGui::SameLine();
        ImGui::Text((std::to_string(remapEvent->mEntity.GetEntityID()) + ")").c_str());
        ImGui::PopStyleColor();

        ImGui::NewLine(); ImGui::NewLine();
        ImGui::Text("New Path: ");
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(0.f, 1.f, 0.f, 1.f));
        ImGui::PushStyleVar(ImGuiStyleVar_DisabledAlpha, 1.f);

        std::string displayTxt{ newPath.empty() ? "No file selected" : newPath.c_str() };
        ImGui::InputText("##NewPath", &displayTxt);
        if (ImGui::IsItemClicked()) {
          newPath = AssetHelpers::LoadFileFromExplorer();
        }

        ImGui::PopStyleVar();
        ImGui::PopStyleColor();

        ImGui::NewLine(); ImGui::NewLine();
        ImGui::BeginDisabled(newPath.empty());
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x * 0.5f - ImGui::CalcTextSize("Cont").x);
        if (ImGui::Button("Continue")) {
          // call on asset manager to map the guid to the new path
          IGE_EVENTMGR.DispatchImmediateEvent<Events::RemapGUID>(remapEvent->mGUID, newPath, remapEvent->mFileType);

          resolved = true;
          ImGui::CloseCurrentPopup();
        }
        ImGui::EndDisabled();

        ImGui::EndPopup();
      }

      EndDaFrame();
    }
  }

  void PopupHelper::GUIDRemapPopup() {
    
  }

} // namespace GUI
