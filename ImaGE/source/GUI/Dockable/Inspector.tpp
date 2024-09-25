template<typename Component>
bool Inspector::WindowBegin(std::string const& windowName, std::string const& icon, bool highlight) {
  ImGui::Separator();

  if (mEntityChanged) {
    bool& openMapStatus = mComponentOpenStatusMap[windowName];
    ImGui::SetNextItemOpen(openMapStatus, ImGuiCond_Always);
  }

  std::string display{ icon + "   " + windowName };

  if (highlight) { ImGui::PushStyleColor(ImGuiCol_Text, sComponentHighlightCol); }
  bool const isOpen{ ImGui::TreeNode(display.c_str())};
  if (highlight) { ImGui::PopStyleColor(); }

  if (isOpen) {
    ImGui::PushFont(mStyler.GetCustomFont(GUI::MONTSERRAT_LIGHT));
    // Must close component window if a component was removed
    if (!DrawOptionsListButton<Component>(windowName)) {
      WindowEnd(true);
      return false;
    }
  }

  mComponentOpenStatusMap[windowName] = isOpen;
  return isOpen;
}

template<typename Component>
void Inspector::DrawAddComponentButton(std::string const& name, std::string const& icon) {
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

  std::string display{ icon + "   " + name};
    
  ImGui::PushFont(mStyler.GetCustomFont(GUI::MONTSERRAT_SEMIBOLD));
  ImGui::TextUnformatted(display.c_str());
  ImGui::PopFont();

  if (isRowHovered)
    fillRowWithColour(Color::IMGUI_COLOR_ORANGE);

  if (isRowClicked) {
    ECS::Entity ent{ GUIManager::GetSelectedEntity().GetRawEnttEntityID() };
    ent.EmplaceComponent<Component>();
    SetIsComponentEdited(true);
    ImGui::CloseCurrentPopup();
  }
}

template<typename Component>
bool Inspector::DrawOptionButton(std::string const& name) {
  bool openMainWindow{ true };
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
      SetIsComponentEdited(true);
      openMainWindow = false;
    }

    else if (name == "Clear") {
      auto& component = ent.GetComponent<Component>();
      SetIsComponentEdited(true);
      component.Clear();
    }

    ImGui::CloseCurrentPopup();
  }

  return openMainWindow;
}

template<typename Component>
bool Inspector::DrawOptionsListButton(std::string const& windowName) {
  bool openMainWindow{ true };
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
        openMainWindow = DrawOptionButton<Component>("Remove Component");

      ImGui::EndTable();
    }

    ImGui::EndPopup();
  }

  return openMainWindow;
}