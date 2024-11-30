#include <pch.h>
#include "PopupHelper.h"
#include <Events/EventManager.h>
#include <Events/AssetEvents.h>
#include <GUI/GUIVault.h>
#include <ImGui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <GUI/Helpers/AssetHelpers.h>
#include <queue>
#include <Asset/RemapData.h>
#include <Scenes/SceneManager.h>

namespace {
  static std::unordered_map<IGE::Assets::GUID, IGE::Assets::RemapData> sGUIDDataMap;  // unique entries of remap data
  static std::queue<IGE::Assets::RemapData> sGUIDData; // guids to remap
  static std::string sNewPath;
  static bool sHideCompletionPopup{ false };

  void DisplayLocalHierarchy(ECS::Entity entity, ImVec4 const& entityClr);
}

namespace GUI {
  PopupHelper::PopupHelper(const char* windowName) : GUIWindow(windowName) {
    mPopupFunctions.emplace_back(std::bind(&PopupHelper::GUIDRemapPopup, this));
    mPopupFunctions.emplace_back(std::bind(&PopupHelper::GUIDCompletePopup, this));

    SUBSCRIBE_CLASS_FUNC(Events::EventType::INVALID_GUID, &PopupHelper::OnGUIDInvalidated, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::TRIGGER_GUID_REMAP, &PopupHelper::OnGUIDRemap, this);
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

  EVENT_CALLBACK_DEF(PopupHelper, OnGUIDInvalidated) {
    auto const& remapEvent{ CAST_TO_EVENT(Events::GUIDInvalidated) };
    // if already exists, ignore
    if (sGUIDDataMap.contains(remapEvent->mGUID)) { return; }

    // queue an entry for remapping
    sGUIDDataMap.emplace(std::piecewise_construct, std::forward_as_tuple(remapEvent->mGUID),
      std::forward_as_tuple(remapEvent->mEntity, remapEvent->mGUID, remapEvent->mFileType));
  }

  EVENT_CALLBACK_DEF(PopupHelper, OnGUIDRemap) {
    // do nothing if queue is empty
    if (sGUIDDataMap.empty()) { return; }

    // move data into queue
    for (auto& data : sGUIDDataMap) {
        sGUIDData.emplace(std::move(data.second));
    }
    sGUIDDataMap.clear();

    // trigger the popup
    sCurrentPopup = sGUIDPopupTitle;
    sOpenPopup = true;
  }


  void PopupHelper::GUIDRemapPopup() {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSizeConstraints(ImVec2(100.0f, 0.0f), ImVec2(FLT_MAX, ImGui::GetMainViewport()->Size.y * 0.8f));

    Styler const& styler{ GUIVault::GetStyler() };
    ImGui::PushFont(styler.GetCustomFont(CustomFonts::ROBOTO_BOLD));
    if (!ImGui::BeginPopupModal(sGUIDPopupTitle, NULL, ImGuiWindowFlags_AlwaysAutoResize)) { ImGui::PopFont(); return; }
    ImGui::PushFont(styler.GetCustomFont(CustomFonts::MONTSERRAT_REGULAR));
    
    static std::string warningText{};
    // always reference the front of the queue
    IGE::Assets::RemapData& remapData{ sGUIDData.front() };
    bool closePopup{ false };
    ImVec4 const redClr{ 1.f, 0.f, 0.f, 1.f };

    // opening text
    ImGui::Text("Congratulations! You have a total of");
    ImGui::SameLine();
    ImGui::TextColored(redClr, std::to_string(sGUIDData.size()).c_str());
    ImGui::SameLine();
    ImGui::Text("broken GUID references remaining!");

    // asset info
    ImGui::NewLine();
    ImGui::Text("We were unable to locate:");
    ImGui::PushStyleColor(ImGuiCol_Text, redClr);
    ImGui::Text((remapData.fileType + " Asset (GUID").c_str());
    ImGui::SameLine();
    ImGui::Text((std::to_string(static_cast<uint64_t>(remapData.guid)) + ")").c_str());
    ImGui::PopStyleColor();

    // entity info
    ImGui::NewLine();
    ImGui::Text("Affected entity's local hierarchy:");
    ImGui::PushFont(styler.GetCustomFont(CustomFonts::MONTSERRAT_LIGHT));
    DisplayLocalHierarchy(remapData.entity, redClr);
    ImGui::PopFont();

    // file selection
    ImGui::NewLine(); ImGui::NewLine();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("New Path:");
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(0.f, 1.f, 0.f, 1.f));
    ImGui::PushStyleVar(ImGuiStyleVar_DisabledAlpha, 1.f);

    std::string displayTxt{ sNewPath.empty() ? "Select file" : sNewPath.c_str() };
    ImGui::BeginDisabled();
    ImGui::InputText("##NewPath", &displayTxt);
    ImGui::EndDisabled();
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) { 
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

      if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        sNewPath = AssetHelpers::LoadFileFromExplorer();
      }
    }

    if (!warningText.empty()) {
      ImGui::TextColored(redClr, warningText.c_str());
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    // buttons
    ImGui::NewLine(); ImGui::NewLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x * 0.5f - ImGui::CalcTextSize("Skip All Skip C").x);

    bool hovered{ false };
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.f, 0.f, 1.f));
    if (ImGui::Button("Skip All")) {
      sGUIDData = {}; // empty the queue
      closePopup = true;
    }
    if (ImGui::IsItemHovered()) { hovered = true; }

    ImGui::SameLine();
    if (ImGui::Button("Skip")) {
      closePopup = true;
    }
    ImGui::PopStyleColor();
    if (ImGui::IsItemHovered()) { hovered = true; }

    if (hovered) {
      ImGui::SetTooltip("You'll get this popup again the next time you load the scene");
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    }

    ImGui::SameLine();
    if (sNewPath.empty()) {
      ImGui::BeginDisabled();
    }
    else {
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.5f, 0.f, 1.f));
    }
    
    if (ImGui::Button("Continue")) {
      try {
        // call on asset manager to map the guid to the new path
        IGE_EVENTMGR.DispatchImmediateEvent<Events::RemapGUID>(remapData.guid, sNewPath, remapData.fileType);

        closePopup = true;
      }
      catch (Debug::ExceptionBase const& e) {
        warningText = e.ErrMsg();
      }
    }

    if (sNewPath.empty()) {
      ImGui::EndDisabled();

      if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("Select a file first!");
      }
    }
    else {
      ImGui::PopStyleColor(); // pop ImGuiCol_Button

      if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
      }
    }

    if (closePopup) {
      // pop the current referenced data
      if (!sGUIDData.empty()) {
        sGUIDData.pop();
      }
      ImGui::CloseCurrentPopup();
      warningText.clear();
      sNewPath.clear();

      // if empty, reload the scene
      if (sGUIDData.empty()) {
        if (!sHideCompletionPopup) {
          sCurrentPopup = sGUIDCompletePopupTitle;
          sOpenPopup = true;
        }
      }
      // loop this popup until all references resolved
      else {
        // trigger the popup
        sCurrentPopup = sGUIDPopupTitle;
        sOpenPopup = true;
      }
    }

    ImGui::PopFont();
    ImGui::EndPopup();
    ImGui::PopFont();
  }

  void PopupHelper::GUIDCompletePopup() {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    Styler const& styler{ GUIVault::GetStyler() };
    ImGui::PushFont(styler.GetCustomFont(CustomFonts::ROBOTO_BOLD));
    if (!ImGui::BeginPopupModal(sGUIDCompletePopupTitle, NULL, ImGuiWindowFlags_AlwaysAutoResize)) { ImGui::PopFont(); return; }
    ImGui::PushFont(styler.GetCustomFont(CustomFonts::MONTSERRAT_REGULAR));

    ImGui::Text("Note: You have to reload the scene to see updated changes");

    ImGui::PushFont(styler.GetCustomFont(CustomFonts::MONTSERRAT_LIGHT));
    ImGui::Checkbox("Don't show again this session", &sHideCompletionPopup);
    ImGui::PopFont();

    ImGui::NewLine();
    if (ImGui::Button("OK")) {
      ImGui::CloseCurrentPopup();
    }
    if (ImGui::IsItemHovered()) { ImGui::SetMouseCursor(ImGuiMouseCursor_Hand); }

    ImGui::PopFont();
    ImGui::EndPopup();
    ImGui::PopFont();
  }

} // namespace GUI

namespace {
  void DisplayEntity(ECS::Entity entity, float cursorXPos) {
    ImGui::SetCursorPosX(cursorXPos);
    ImGui::Text((entity.GetTag() + " (Entity").c_str());
    ImGui::SameLine();
    ImGui::Text((std::to_string(entity.GetEntityID()) + ")").c_str());
  }

  void DisplayLocalHierarchy(ECS::Entity entity, ImVec4 const& entityClr) {
    ECS::EntityManager& em{ IGE_ENTITYMGR };
    float cursorXPos{ ImGui::GetCursorPosX() };

    // if entity has parent, display as first level
    if (em.HasParent(entity)) {
      ECS::Entity const parent{ em.GetParentEntity(entity) };
      DisplayEntity(parent, cursorXPos);
      // indent cursorX by tree node indent width
      cursorXPos += ImGui::GetTreeNodeToLabelSpacing();

      for (ECS::Entity child : em.GetChildEntity(parent)) {
        // highlight the target entity
        if (child == entity) {
          ImGui::PushStyleColor(ImGuiCol_Text, entityClr);
          DisplayEntity(child, cursorXPos);
          ImGui::PopStyleColor();

          if (em.HasChild(entity)) {
            // indent cursorX by tree node indent width again
            cursorXPos += ImGui::GetTreeNodeToLabelSpacing();
            for (ECS::Entity c : em.GetChildEntity(entity)) {
              DisplayEntity(c, cursorXPos);
            }
          }

          continue;
        }

        DisplayEntity(child, cursorXPos);
      }
    }
    // no parent: just display entity on first lvl
    else {
      ImGui::PushStyleColor(ImGuiCol_Text, entityClr);
      DisplayEntity(entity, cursorXPos);
      ImGui::PopStyleColor();

      if (em.HasChild(entity)) {
        // indent cursorX by tree node indent width again
        cursorXPos += ImGui::GetTreeNodeToLabelSpacing();
        for (ECS::Entity child : em.GetChildEntity(entity)) {
          DisplayEntity(child, cursorXPos);
        }
      }
    }
  }
}
