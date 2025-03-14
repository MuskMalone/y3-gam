template<typename Component>
bool Inspector::WindowBegin(std::string const& windowName, bool highlight) {
  ImGui::Separator();

  std::string const display{ mComponentIcons.at(typeid(Component)) + windowName };

  if (highlight) { ImGui::PushStyleColor(ImGuiCol_Text, sComponentHighlightCol); }
  if (mEntityChanged) {
    bool& openMapStatus = mComponentOpenStatusMap[windowName];
    ImGui::SetNextItemOpen(openMapStatus, ImGuiCond_Always);
  }
  bool const isOpen{ ImGui::TreeNodeEx(display.c_str()) };
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

template<typename ComponentType>
bool Inspector::DrawAddComponentButton(std::string const& name) {
  if (GUIVault::GetSelectedEntity().HasComponent<ComponentType>()) {
    return false;
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

  std::string const display{ mComponentIcons.at(typeid(ComponentType)) + name};
    
  ImGui::PushFont(mStyler.GetCustomFont(GUI::MONTSERRAT_SEMIBOLD));
  ImGui::TextUnformatted(display.c_str());
  ImGui::PopFont();

  if (isRowHovered)
    fillRowWithColour(Color::IMGUI_COLOR_ORANGE);

  if (isRowClicked) {
    ECS::Entity ent{ GUIVault::GetSelectedEntity().GetRawEnttEntityID() };
    if constexpr (std::is_same<ComponentType, Component::RigidBody>::value) {
        ComponentType& newComp{ IGE::Physics::PhysicsSystem::GetInstance()->AddRigidBody(ent) };
    } else if constexpr (std::is_same<ComponentType, Component::BoxCollider>::value) {
        ComponentType& newComp{ IGE::Physics::PhysicsSystem::GetInstance()->AddBoxCollider(ent, true) };
    } else if constexpr (std::is_same<ComponentType, Component::SphereCollider>::value) {
        ComponentType& newComp{ IGE::Physics::PhysicsSystem::GetInstance()->AddSphereCollider(ent, true) };
    }else if constexpr (std::is_same<ComponentType, Component::CapsuleCollider>::value) {
        ComponentType& newComp{ IGE::Physics::PhysicsSystem::GetInstance()->AddCapsuleCollider(ent, true) };
    }else {
        ComponentType& newComp{ ent.EmplaceComponent<ComponentType>() };
    }
    SetIsComponentEdited(true);
    mEntityChanged = true;  // allow added component to be open on first instance

    // if entity is a prefab instance, update its modified components
    if (ent.HasComponent<Component::PrefabOverrides>()) {
      ent.GetComponent<Component::PrefabOverrides>().AddComponentOverride<ComponentType>();
    }

    ImGui::CloseCurrentPopup();

    return true;
  }

  return false;
}

template<typename ComponentType>
bool Inspector::DrawOptionsListButton(std::string const& windowName) {
  bool openMainWindow{ true };
  //ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.f);
  static ImVec2 const addTextSize = ImGui::CalcTextSize(ICON_FA_ELLIPSIS_VERTICAL);
  float const contentRegionAvailableX = ImGui::GetContentRegionAvail().x;
  float const paddingX = 5.f;
  float const buttonWidth = addTextSize.x + paddingX * 2.0f;
  ImGui::SameLine(contentRegionAvailableX - addTextSize.x - paddingX);

  ImGui::PushStyleColor(ImGuiCol_Button, 0);
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, 0);
  if (ImGui::Button(ICON_FA_ELLIPSIS_VERTICAL, ImVec2(buttonWidth, 0))) {
    ImVec2 const buttonPos = ImGui::GetItemRectMin();
    ImVec2 const buttonSize = ImGui::GetItemRectSize();

    ImVec2 const popupPos(buttonPos.x, buttonPos.y + buttonSize.y);
    ImGui::SetNextWindowPos(popupPos);

    ImGui::OpenPopup("OptionsPanel");
  }
  ImGui::PopStyleColor(2);
  if (ImGui::IsItemHovered()) {
    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
  }

  if (ImGui::BeginPopup("OptionsPanel", ImGuiWindowFlags_NoMove)) {
    if (ImGui::BeginTable("##options_table", 1, ImGuiTableFlags_SizingStretchSame)) {
      ImGui::TableSetupColumn("OptionNames", ImGuiTableColumnFlags_WidthFixed, 200.f);

      ECS::Entity ent{ GUIVault::GetSelectedEntity() };
      Component::PrefabOverrides* overrides{ ent.HasComponent<Component::PrefabOverrides>() ?
        &ent.GetComponent<Component::PrefabOverrides>() : nullptr };

      // Clear component
      if (DrawOptionButton("Clear")) {
        ent.GetComponent<ComponentType>().Clear();
        if (std::is_same<Component::Layer, ComponentType>()) {
          ent.SetLayer("Default");
        }

        // if its a prefab instance, add to overrides
        if (overrides) {
          overrides->AddComponentOverride<ComponentType>();
        }

        SetIsComponentEdited(true);
        ImGui::CloseCurrentPopup();
      }

      // Remove component
      if (!std::is_same<Component::Tag, ComponentType>() && !std::is_same<Component::Layer, ComponentType>()
        && !std::is_same<Component::Transform, ComponentType>()) {
        if (DrawOptionButton("Remove Component")) {
          ent.GetComponent<ComponentType>().Clear();
          ent.RemoveComponent<ComponentType>();

          // if its a prefab instance, add to overrides
          if (overrides) {
            overrides->AddComponentRemoval<ComponentType>();
          }

          openMainWindow = false;
          SetIsComponentEdited(true);
          ImGui::CloseCurrentPopup();
        }
      }

      // Reset component overrides
      if (overrides && overrides->IsComponentModified<ComponentType>()) {
        // remove the overriden component and restore the prefab's original
        if (DrawOptionButton("Reset Overrides")) {
          overrides->RemoveOverride<ComponentType>();
          try {
            Prefabs::Prefab const& pfb{ IGE_ASSETMGR.GetAsset<IGE::Assets::PrefabAsset>(overrides->guid)->mPrefabData };
            rttr::variant const& compToRestore{ pfb.GetSubObject(overrides->subDataId).GetComponent<ComponentType>() };
            if (compToRestore.is_valid()) {
              IGE_OBJFACTORY.AddComponentToEntity(ent, compToRestore);
            }
            else {
              ent.RemoveComponent<ComponentType>();
              openMainWindow = false;
            }
          }
          catch (Debug::ExceptionBase&) {
            IGE_DBGLOGGER.LogError("Unable to fetch prefab " + std::to_string(overrides->guid));
          }

          SetIsComponentEdited(true);
          ImGui::CloseCurrentPopup();
        }
      }

      ImGui::EndTable();
    }

    ImGui::EndPopup();
  }

  return openMainWindow;
}