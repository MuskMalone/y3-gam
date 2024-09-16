#include <pch.h>

#include "Inspector.h"
#include "Color.h"
#include "GUI/Helpers/ImGuiHelpers.h"

namespace GUI {
  template<typename Component>
  void DrawAddComponentButton(std::string const& name);

  template<typename Component>
  void DrawOptionButton(std::string const& name);

  template<typename Component>
  void DrawOptionsListButton(std::string windowName);

  // Static Initialization
  std::map<std::string, bool> Inspector::sComponentOpenStatusMap{};
  ECS::Entity Inspector::sPreviousEntity{};
  bool Inspector::sEntityChanged{};
  bool Inspector::sIsComponentEdited{ false };

  Inspector::Inspector(std::string const& name) : GUIWindow(name) {
    for (std::string const& component : Component::ComponentNameList) {
      sComponentOpenStatusMap[component] = true; // Default to all open
    }
  }

  void Inspector::Run() {
    ImGui::Begin(mWindowName.c_str());
    ECS::Entity const& currentEntity{ GUIManager::GetSelectedEntity() };
    if (currentEntity) {
      if (currentEntity != sPreviousEntity) {
        sPreviousEntity = currentEntity;
        sEntityChanged = true;
      }
      else
        sEntityChanged = false;

      // @TODO: EDIT WHEN NEW COMPONENTS
      if (currentEntity.HasComponent<Component::Tag>())
        TagComponentWindow(currentEntity);

      if (currentEntity.HasComponent<Component::Layer>())
        LayerComponentWindow(currentEntity);

      if (currentEntity.HasComponent<Component::Script>())
        ScriptComponentWindow(currentEntity);

      if (currentEntity.HasComponent<Component::Text>())
        TextComponentWindow(currentEntity);

      if (currentEntity.HasComponent<Component::Transform>())
        TransformComponentWindow(currentEntity);
    }

    ImGui::End();
  }

  bool const Inspector::GetIsComponentEdited() {
    return sIsComponentEdited;
  }

  void Inspector::SetIsComponentEdited(bool isComponentEdited) {
    sIsComponentEdited = isComponentEdited;
  }

  void Inspector::LayerComponentWindow(ECS::Entity entity) {
    bool isOpen{ WindowBegin<Component::Layer>("Layer") };

    if (isOpen) {
      
    }

    WindowEnd(isOpen);
  }

  void Inspector::ScriptComponentWindow(ECS::Entity entity) {
    bool isOpen{ WindowBegin<Component::Script>("Script") };

    if (isOpen) {

    }

    WindowEnd(isOpen);
  }

  void Inspector::TagComponentWindow(ECS::Entity entity) {
    bool isOpen{ WindowBegin<Component::Tag>("Tag") };

    if (isOpen) {
      std::string tag{ entity.GetTag() };
      if (ImGui::InputText("##Tag", &tag, ImGuiInputTextFlags_EnterReturnsTrue)) {
        entity.SetTag(tag);
        SetIsComponentEdited(true);
      }

      DrawAddButton();
    }

    WindowEnd(isOpen);
  }

  void Inspector::TextComponentWindow(ECS::Entity entity) {
    bool isOpen{ WindowBegin<Component::Text>("Text") };

    if (isOpen) {
      
    }

    WindowEnd(isOpen);
  }

  void Inspector::TransformComponentWindow(ECS::Entity entity) {
    bool isOpen{ WindowBegin<Component::Transform>("Transform") };

    if (isOpen) {
      auto& transform = entity.GetComponent<Component::Transform>();

      float contentSize = ImGui::GetContentRegionAvail().x;
      float charSize = ImGui::CalcTextSize("012345678901234").x;
      float inputWidth = (contentSize - charSize - 30) / 3;

      /*
      ImGui::BeginTable("TransformTable", 2, ImGuiTableFlags_BordersInnerV);
      
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, charSize);
      ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, inputWidth); // Fixed width for input fields
      ImGui::TableHeadersRow(); // Optional header row

      // Translation
      ImGui::TableNextRow();
      if (ImGuiHelpers::InputDouble3("Translation", transform.localPos, inputWidth, false)) {
        SetIsComponentEdited(true);
      }

      // Rotation
      ImGui::TableNextRow();
      if (ImGuiHelpers::InputDouble3("Rotation", transform.localRot, inputWidth, false)) {
        SetIsComponentEdited(true);
      }

      // Scale
      ImGui::TableNextRow();
      if (ImGuiHelpers::InputDouble3("Scale", transform.localScale, inputWidth, false)) {
        SetIsComponentEdited(true);
      }
      */

      // TEMP
      ImGui::BeginTable("TransformTable", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, charSize);
      ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableHeadersRow();

      // Translation
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Translation");
      ImGui::TableSetColumnIndex(1);
      ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red
      ImGui::InputDouble("##XTranslation", &transform.localPos.x, 0.0, 0.0, "%.3f");
      ImGui::PopStyleColor();

      ImGui::TableSetColumnIndex(2);
      ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // Green
      ImGui::InputDouble("##YTranslation", &transform.localPos.y, 0.0, 0.0, "%.3f");
      ImGui::PopStyleColor();

      ImGui::TableSetColumnIndex(3);
      ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 1.0f, 1.0f)); // Blue
      ImGui::InputDouble("##ZTranslation", &transform.localPos.z, 0.0, 0.0, "%.3f");
      ImGui::PopStyleColor();

      // Rotation
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Rotation");
      ImGui::TableSetColumnIndex(1);
      ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red
      ImGui::InputDouble("##XRotation", &transform.localRot.x, 0.0, 0.0, "%.3f");
      ImGui::PopStyleColor();

      ImGui::TableSetColumnIndex(2);
      ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // Green
      ImGui::InputDouble("##YRotation", &transform.localRot.y, 0.0, 0.0, "%.3f");
      ImGui::PopStyleColor();

      ImGui::TableSetColumnIndex(3);
      ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 1.0f, 1.0f)); // Blue
      ImGui::InputDouble("##ZRotation", &transform.localRot.z, 0.0, 0.0, "%.3f");
      ImGui::PopStyleColor();

      // Scale
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Scale");
      ImGui::TableSetColumnIndex(1);
      ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red
      ImGui::InputDouble("##XScale", &transform.localScale.x, 0.0, 0.0, "%.3f");
      ImGui::PopStyleColor();

      ImGui::TableSetColumnIndex(2);
      ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // Green
      ImGui::InputDouble("##YScale", &transform.localScale.y, 0.0, 0.0, "%.3f");
      ImGui::PopStyleColor();

      ImGui::TableSetColumnIndex(3);
      ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 1.0f, 1.0f)); // Blue
      ImGui::InputDouble("##ZScale", &transform.localScale.z, 0.0, 0.0, "%.3f");
      ImGui::PopStyleColor();

      ImGui::EndTable();
    }

    WindowEnd(isOpen);
  }

  void Inspector::WindowEnd(bool isOpen) {
    if (isOpen)
      ImGui::TreePop();

    ImGui::Separator();
  }

  void Inspector::DrawAddButton() {
    ImVec2 addTextSize = ImGui::CalcTextSize("Add");
    ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
    float paddingX = 5.0f;
    float buttonWidth = addTextSize.x + paddingX * 2.0f;
    ImGui::SameLine(contentRegionAvailable.x - addTextSize.x);
    if (ImGui::Button("Add", ImVec2(buttonWidth, 0))) {
      ImVec2 buttonPos = ImGui::GetItemRectMin();
      ImVec2 buttonSize = ImGui::GetItemRectSize();

      ImVec2 popupPos(buttonPos.x, buttonPos.y + buttonSize.y);
      ImGui::SetNextWindowPos(popupPos);

      ImGui::OpenPopup("AddComponentPanel");
    }

    if (ImGui::BeginPopup("AddComponentPanel", ImGuiWindowFlags_NoMove)) {

      if (ImGui::BeginTable("##component_table", 1, ImGuiTableFlags_SizingStretchSame)) {
        ImGui::TableSetupColumn("ComponentNames", ImGuiTableColumnFlags_WidthFixed, 200.f);
        // @TODO: EDIT WHEN NEW COMPONENTS
        DrawAddComponentButton<Component::Layer>("Layer");
        DrawAddComponentButton<Component::Script>("Script");
        DrawAddComponentButton<Component::Tag>("Tag");
        DrawAddComponentButton<Component::Text>("Text");
        DrawAddComponentButton<Component::Transform>("Transform");  
        ImGui::EndTable();
      }

      ImGui::EndPopup();
    }
  }

  template<typename Component>
  void DrawAddComponentButton(std::string const& name) {
    if (GUIManager::GetSelectedEntity().HasComponent<Component>()) {
      return;
    }
    
    auto fillRowWithColour = [](const ImColor& colour) {
      for (int column = 0; column < ImGui::TableGetColumnCount(); column++) {
        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, colour, column);
      }
    };
    
    const float rowHeight = 25.0f;
    auto* window = ImGui::GetCurrentWindow();
    window->DC.CurrLineSize.y = rowHeight;
    ImGui::TableNextRow(0, rowHeight);
    ImGui::TableSetColumnIndex(0);

    window->DC.CurrLineTextBaseOffset = 3.0f;

    const ImVec2 rowAreaMin = ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 0).Min;
    const ImVec2 rowAreaMax = { ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 
      ImGui::TableGetColumnCount() - 1).Max.x, rowAreaMin.y + rowHeight };

    //ImGui::GetWindowDrawList()->AddRect(rowAreaMin, rowAreaMax, Color::IMGUI_COLOR_RED); // Debug

    ImGui::PushClipRect(rowAreaMin, rowAreaMax, false);
    bool isRowHovered, isRowClicked;
    ImGui::ButtonBehavior(ImRect(rowAreaMin, rowAreaMax), ImGui::GetID(name.c_str()), 
      &isRowHovered, &isRowClicked, ImGuiButtonFlags_MouseButtonLeft);
    ImGui::SetItemAllowOverlap();
    ImGui::PopClipRect();

    ImGui::TextUnformatted(name.c_str());

    if (isRowHovered)
      fillRowWithColour(Color::IMGUI_COLOR_ORANGE);

    if (isRowClicked) {
      ECS::Entity ent{ GUIManager::GetSelectedEntity().GetRawEnttEntityID() };
      ent.EmplaceComponent<Component>();
      ImGui::CloseCurrentPopup();
    }
  }

  template<typename Component>
  void DrawOptionButton(std::string const& name) {
    auto fillRowWithColour = [](const ImColor& colour) {
      for (int column = 0; column < ImGui::TableGetColumnCount(); column++) {
        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, colour, column);
      }
    };

    const float rowHeight = 25.0f;
    auto* window = ImGui::GetCurrentWindow();
    window->DC.CurrLineSize.y = rowHeight;
    ImGui::TableNextRow(0, rowHeight);
    ImGui::TableSetColumnIndex(0);

    window->DC.CurrLineTextBaseOffset = 3.0f;

    const ImVec2 rowAreaMin = ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 0).Min;
    const ImVec2 rowAreaMax = { ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(),
      ImGui::TableGetColumnCount() - 1).Max.x, rowAreaMin.y + rowHeight };

    //ImGui::GetWindowDrawList()->AddRect(rowAreaMin, rowAreaMax, Color::IMGUI_COLOR_RED); // Debug

    ImGui::PushClipRect(rowAreaMin, rowAreaMax, false);
    bool isRowHovered, isRowClicked;
    ImGui::ButtonBehavior(ImRect(rowAreaMin, rowAreaMax), ImGui::GetID(name.c_str()),
      &isRowHovered, &isRowClicked, ImGuiButtonFlags_MouseButtonLeft);
    ImGui::SetItemAllowOverlap();
    ImGui::PopClipRect();

    ImGui::TextUnformatted(name.c_str());

    if (isRowHovered)
      fillRowWithColour(Color::IMGUI_COLOR_RED);

    if (isRowClicked) {
      ECS::Entity ent{ GUIManager::GetSelectedEntity().GetRawEnttEntityID() };

      if (name == "Remove Component") {
        ent.RemoveComponent<Component>();
      }

      else if (name == "Clear") {
        auto& component = ent.GetComponent<Component>();
        component.Clear();
      }

      ImGui::CloseCurrentPopup();
    }
  }

  template<typename Component>
  void DrawOptionsListButton(std::string windowName) {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 25.f);
    ImVec2 addTextSize = ImGui::CalcTextSize("Options");
    ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
    float paddingX = 10.0f;
    float buttonWidth = addTextSize.x + paddingX * 2.0f;
    ImGui::SameLine(contentRegionAvailable.x - addTextSize.x - paddingX);

    if (ImGui::Button("Options", ImVec2(buttonWidth, 0))) {
      ImVec2 buttonPos = ImGui::GetItemRectMin();
      ImVec2 buttonSize = ImGui::GetItemRectSize();

      ImVec2 popupPos(buttonPos.x, buttonPos.y + buttonSize.y);
      ImGui::SetNextWindowPos(popupPos);

      ImGui::OpenPopup("OptionsPanel");
    }

    ImGui::PopStyleVar();

    if (ImGui::BeginPopup("OptionsPanel", ImGuiWindowFlags_NoMove)) {

      if (ImGui::BeginTable("##options_table", 1, ImGuiTableFlags_SizingStretchSame)) {
        ImGui::TableSetupColumn("OptionNames", ImGuiTableColumnFlags_WidthFixed, 200.f);
        DrawOptionButton<Component>("Clear");
        if (windowName != "Tag")
          DrawOptionButton<Component>("Remove Component");

        ImGui::EndTable();
      }

      ImGui::EndPopup();
    }
  }
} // namespace GUI