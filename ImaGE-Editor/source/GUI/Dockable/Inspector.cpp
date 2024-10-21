/*!*********************************************************************
\file   Inspector.cpp
\author 
\date   5-October-2024
\brief  Class encapsulating functions to run the inspector / property
        window of the editor. Displays and allows modification of
        components for the currently selected entity.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "Inspector.h"

#include <imgui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include "Color.h"
#include "GUI/Helpers/ImGuiHelpers.h"
#include <GUI/Helpers/AssetPayload.h>
#include <Core/Systems/TransformSystem/TransformHelpers.h>
#include <Reflection/Metadata.h>

#include "Physics/PhysicsSystem.h"
#include <functional>
#include <Reflection/ComponentTypes.h>
#include <Core/Components/Components.h>
#include <Events/EventManager.h>
#include <Graphics/MeshFactory.h>
#include <Graphics/Mesh.h>
#include "Asset/IGEAssets.h"

#define GET_RTTR_TYPE(T) rttr::type::get<T>()
#define CHECK_RTTR_TYPE(rttrType,T) rttrType == rttr::type::get<T>()
#define ICON_PADDING "   "

namespace {
  static bool sIsComponentEdited{ false }, sFirstEdit{false};
  static std::unordered_map<rttr::type, bool> sEntityComponentMap;

  // component icons
  std::unordered_map<rttr::type, const char*> const sComponentIcons{
    { GET_RTTR_TYPE(Component::Tag), ICON_FA_TAG ICON_PADDING },
    { GET_RTTR_TYPE(Component::Transform), ICON_FA_ROTATE ICON_PADDING },
    { GET_RTTR_TYPE(Component::BoxCollider), ICON_FA_BOMB ICON_PADDING },
    { GET_RTTR_TYPE(Component::Layer), ICON_FA_LAYER_GROUP ICON_PADDING },
    { GET_RTTR_TYPE(Component::Material), ICON_FA_GEM ICON_PADDING },
    { GET_RTTR_TYPE(Component::Mesh), ICON_FA_CUBE ICON_PADDING },
    { GET_RTTR_TYPE(Component::RigidBody), ICON_FA_CAR ICON_PADDING },
    { GET_RTTR_TYPE(Reflection::ProxyScriptComponent), ICON_FA_FILE_CODE ICON_PADDING },
    { GET_RTTR_TYPE(Component::Text), ICON_FA_FONT ICON_PADDING }
  };

  inline void SetIsComponentEdited(bool isComponentEdited) noexcept { sIsComponentEdited = isComponentEdited; }

  /*!*********************************************************************
  \brief
    Draws the add component option for a particular component
  \param name
    The name of the component
  \param component
    The component in the form of an rttr::variant
  \param compType
    The type of the component
  \return
    True if the main window should be opened and false otherwise
  ************************************************************************/
  bool DrawOptionsListButton(std::string const& name, rttr::variant& component, rttr::type const& compType);

  /*!*********************************************************************
  \brief
    Draws the options for a particular component
  \param name
    The name of the component
  \param component
    The component in the form of an rttr::variant
  \param compType
    The type of the component
  \return
    True if the main window should be opened and false otherwise
  ************************************************************************/
  bool DrawOptionButton(std::string const& name, rttr::variant& component, rttr::type const& compType);

  void ResetComponentMap(std::vector<rttr::variant> const& entityComponents);
}

namespace GUI {


  bool InputDouble3(std::string propertyName, glm::dvec3& property, float fieldWidth, bool disabled = false);

  Inspector::Inspector(const char* name) : GUIWindow(name),
    mComponentOpenStatusMap{}, mStyler{ GUIManager::GetStyler() }, mObjFactory{Reflection::ObjectFactory::GetInstance()},
    mPreviousEntity{}, mEditingPrefab{ false }, mEntityChanged{ false } {
    for (auto const& component : Reflection::gComponentTypes) {
      mComponentOpenStatusMap[component.get_name().to_string().c_str()] = true;
    }

    // set initial flags of all components to false
    sEntityComponentMap.reserve(Reflection::gComponentTypes.size());
    for (rttr::type const& type : Reflection::gComponentTypes) {
      if (CHECK_RTTR_TYPE(type, Component::Script)) {
        sEntityComponentMap.emplace(GET_RTTR_TYPE(Reflection::ProxyScriptComponent), false);
      }
      else {
        sEntityComponentMap.emplace(type, false);
      }
    }

    // get notified when scene is saved
    SUBSCRIBE_CLASS_FUNC(Events::EventType::SAVE_SCENE, &Inspector::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::SCENE_STATE_CHANGE, &Inspector::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::EDIT_PREFAB, &Inspector::HandleEvent, this);
  }

  void Inspector::Run() {
    ImGui::Begin(mWindowName.c_str());
    ImGui::PushFont(mStyler.GetCustomFont(GUI::MONTSERRAT_SEMIBOLD));
    ECS::Entity currentEntity{ GUIManager::GetSelectedEntity() };

    // only update when an entity is selected
    if (!currentEntity) {
      ImGui::PopFont();
      ImGui::End();
      return;
    }

    if (currentEntity != mPreviousEntity) {
      mPreviousEntity = currentEntity;
      mEntityChanged = true;
    }
    else {
      mEntityChanged = false;
    }

    Component::PrefabOverrides* prefabOverride{ nullptr };
    bool componentOverriden{ false };
    if (!mEditingPrefab && currentEntity.HasComponent<Component::PrefabOverrides>()) {
      prefabOverride = &currentEntity.GetComponent<Component::PrefabOverrides>();
      ImGui::PushFont(mStyler.GetCustomFont(GUI::MONTSERRAT_REGULAR));
      ImGui::Text("Prefab instance of");
      ImGui::SameLine();
      ImGui::PushStyleColor(ImGuiCol_Text, sComponentHighlightCol);
      ImGui::Text(prefabOverride->prefabName.c_str());
      ImGui::PopStyleColor();
      ImGui::PopFont();
    }

    // retrieve current entity's components
    std::vector<rttr::variant> entityComponents{ Reflection::ObjectFactory::GetInstance().GetEntityComponents(currentEntity) };
    if (mEntityChanged) {
      ResetComponentMap(entityComponents);
    }

    // iterate through each component's properties and
    // display in the window based on its type
    for (rttr::variant& component : entityComponents) {

      // get underlying type if it's wrapped in a pointer
      rttr::instance compInst{ component };
      rttr::instance wrappedInst{ compInst.get_type().get_raw_type().is_wrapper() ? compInst.get_wrapped_instance() : compInst };

      rttr::type compType{ wrappedInst.get_type().get_raw_type() };
      componentOverriden = prefabOverride && prefabOverride->IsComponentModified(compType);

      bool const isOpen{ WindowBegin(component, compType, componentOverriden) };
      if (isOpen) {
        DisplayProperties(wrappedInst);

        // only display add on Tag row
        if (CHECK_RTTR_TYPE(compType, Component::Tag)) {
          DrawAddButton();
        }
      }

      WindowEnd(isOpen);
    }

    // if its a prefab instance, grey out the removed components
    if (prefabOverride) {
      for (rttr::type const& type : prefabOverride->removedComponents) {
        DisplayRemovedComponent(type);
      }
    }

    ImGui::PopFont();
    ImGui::End();

    // if edit is the first of this session, dispatch a SceneModifiedEvent
    if (!sFirstEdit && sIsComponentEdited) {
      QUEUE_EVENT(Events::SceneModifiedEvent);
      sFirstEdit = true;
    }
  }

  EVENT_CALLBACK_DEF(Inspector, HandleEvent) {
    switch (event->GetCategory()) {
    case Events::EventType::SAVE_SCENE:
      sIsComponentEdited = sFirstEdit = false;
      break;
    case Events::EventType::SCENE_STATE_CHANGE:
    {
      auto state{ CAST_TO_EVENT(Events::SceneStateChange)->mNewState };
      // if changing to another scene, reset modified flag
      if (state == Events::SceneStateChange::CHANGED) {
        sIsComponentEdited = sFirstEdit = mEditingPrefab = false;
      }
      else if (state == Events::SceneStateChange::NEW) {
        sIsComponentEdited = true;
        sFirstEdit = mEditingPrefab = false;
      }
      break;
    }
    case Events::EventType::EDIT_PREFAB:
      mEditingPrefab = true;
      break;
    default: break;
    }
  }

  void Inspector::DisplayRemovedComponent(rttr::type const& type) {
    ImGui::BeginDisabled();
    ImGui::TreeNode((type.get_name().to_string() + " (removed)").c_str());
    ImGui::EndDisabled();
  }

  void Inspector::DrawAddButton() {
    ImVec2 addTextSize = ImGui::CalcTextSize("Add");
    ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
    float paddingX = 5.0f;
    float buttonWidth = addTextSize.x + paddingX * 2.0f;
    ImGui::SameLine(contentRegionAvailable.x - addTextSize.x);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
    ImGui::PushFont(mStyler.GetCustomFont(GUI::MONTSERRAT_SEMIBOLD));
    
    if (ImGui::Button(ICON_FA_CIRCLE_PLUS)) {
      ImVec2 buttonPos = ImGui::GetItemRectMin();
      ImVec2 buttonSize = ImGui::GetItemRectSize();

      ImVec2 popupPos(buttonPos.x, buttonPos.y + buttonSize.y);
      ImGui::SetNextWindowPos(popupPos);

      ImGui::OpenPopup("AddComponentPanel");
    }

    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    if (ImGui::BeginPopup("AddComponentPanel", ImGuiWindowFlags_NoMove)) {

      if (ImGui::BeginTable("##component_table", 1, ImGuiTableFlags_SizingStretchSame)) {
        ImGui::TableSetupColumn("ComponentNames", ImGuiTableColumnFlags_WidthFixed, 200.f);
        
        // @TODO: Retain alphabetical order
        // display components that do not exist
        for (auto const& [type, flag] : sEntityComponentMap) {
          if (flag) { continue; }

          DrawAddComponentButton(type);
        }

        ImGui::EndTable();
      }

      ImGui::EndPopup();
    }
  }

  bool Inspector::WindowBegin(rttr::variant& component, rttr::type const& compType, bool highlight) {
    ImGui::Separator();

    // get underlying type if it's wrapped in a pointer
    std::string const compName{ compType.get_name().to_string() };

    if (mEntityChanged) {
      bool& openMapStatus = mComponentOpenStatusMap[compName];
      ImGui::SetNextItemOpen(openMapStatus, ImGuiCond_Always);
    }

    // this shouldn't be necessary on a stable build
#ifdef _DEBUG
    if (!sComponentIcons.contains(compType)) {
      throw Debug::Exception<Inspector>(Debug::LVL_CRITICAL, Msg("Unable to find icon for " + compName));
    }
#endif

    if (highlight) { ImGui::PushStyleColor(ImGuiCol_Text, sComponentHighlightCol); }
    bool const isOpen{ ImGui::TreeNode((sComponentIcons.at(compType) + compName).c_str()) };
    if (highlight) { ImGui::PopStyleColor(); }

    if (isOpen) {
      ImGui::PushFont(mStyler.GetCustomFont(GUI::MONTSERRAT_LIGHT));
      // Must close component window if a component was removed
      if (!DrawOptionsListButton(compName, component, compType)) {
        WindowEnd(true);
      }
    }

    mComponentOpenStatusMap[compName] = isOpen;
    return isOpen;
  }

  void Inspector::DrawAddComponentButton(rttr::type const& compType) {
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
    std::string const compName{ compType.get_name().to_string() };

    ImGui::PushClipRect(rowAreaMin, rowAreaMax, false);
    bool isRowHovered, isRowClicked;
    ImGui::ButtonBehavior(ImRect(rowAreaMin, rowAreaMax), ImGui::GetID(compName.c_str()),
      &isRowHovered, &isRowClicked, ImGuiButtonFlags_MouseButtonLeft);
    ImGui::SetItemAllowOverlap();
    ImGui::PopClipRect();

    ImGui::PushFont(mStyler.GetCustomFont(GUI::MONTSERRAT_SEMIBOLD));
    ImGui::TextUnformatted((sComponentIcons.at(compType) + compName).c_str());
    ImGui::PopFont();

    if (isRowHovered) {
      fillRowWithColour(Color::IMGUI_COLOR_ORANGE);
    }

    if (isRowClicked) {
      ECS::Entity ent{ GUIManager::GetSelectedEntity().GetRawEnttEntityID() };
      // default construct the component and add it to the entity
      Reflection::ObjectFactory::GetInstance().AddComponentToEntity(ent, compType.create());
      sEntityComponentMap[compType] = true;
      SetIsComponentEdited(true);

      // if entity is a prefab instance, update its modified components
      if (ent.HasComponent<Component::PrefabOverrides>()) {
        ent.GetComponent<Component::PrefabOverrides>().AddComponentModification(compType);
      }
      ImGui::CloseCurrentPopup();
    }
  }

  bool InputDouble3(std::string propertyName, glm::dvec3& property, float fieldWidth, bool disabled)
  {
    bool valChanged{ false };

    ImGui::BeginDisabled(disabled);
    ImGui::TableNextColumn();
    ImGui::Text(propertyName.c_str());
    propertyName = "##" + propertyName;
    ImGui::TableNextColumn();
    ImGui::SetNextItemWidth(fieldWidth);
    if (ImGui::InputDouble((propertyName + "X").c_str(), &property.x, 0, 0, "%.5f")) { valChanged = true; };
    ImGui::SameLine(0, 3); ImGui::SetNextItemWidth(fieldWidth); if (ImGui::InputDouble((propertyName + "Y").c_str(), &property.y, 0, 0, "%.5f")) { valChanged = true; };
    ImGui::SameLine(0, 3); ImGui::SetNextItemWidth(fieldWidth); if (ImGui::InputDouble((propertyName + "Z").c_str(), &property.z, 0, 0, "%.5f")) { valChanged = true; };
    ImGui::EndDisabled();

    return valChanged;
  }

  void Inspector::WindowEnd(bool const isOpen) {
    if (isOpen) {
      ImGui::TreePop();
      ImGui::PopFont();
    }

    ImGui::Separator();
  }

  void Inspector::DisplayProperties(rttr::instance compInst) {
    auto const properties{ compInst.get_type().get_properties() };
    float contentSize = ImGui::GetContentRegionAvail().x;
    float charSize = ImGui::CalcTextSize("012345678901234").x;
    float inputWidth = (contentSize - charSize - 60);

    ImGui::BeginTable(compInst.get_type().get_name().to_string().c_str(), 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

    ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
    ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth);

    // iterate through properties and display based on the type
    for (auto const& property : properties)
    {
      //if (property.get_metadata("NO_SERIALIZE")) { continue; }
      rttr::variant propVal{ property.get_value(compInst) };
      if (!propVal) {
        std::ostringstream oss{};
        oss << "[Inspector] Unable to read property " << property.get_name().to_string() << " of " << compInst.get_type().get_name().to_string() << " component";
        Debug::DebugLogger::GetInstance().LogError(oss.str());
        continue;
      }

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text(property.get_name().to_string().c_str());

      ImGui::TableSetColumnIndex(1);
      ImGui::SetNextItemWidth(INPUT_SIZE);

      rttr::type const propType{ property.get_type() };
      bool const disabled{ property.get_metadata(Reflection::DISABLED) };
      if (disabled) { ImGui::BeginDisabled(); }

      if (CHECK_RTTR_TYPE(propType, std::string)) {
        
      }
      else if (CHECK_RTTR_TYPE(propType, glm::vec3)) {
        //Vec3Input(property, propVal.get_value<glm::vec3>());
      }

      if (disabled) { ImGui::EndDisabled(); }
    }

    ImGui::EndTable();
  }
 
  bool Inspector::Vec3Input(rttr::property const& prop, glm::vec3& data) {
    bool modified{ false };
    float contentSize = ImGui::GetContentRegionAvail().x;
    float charSize = ImGui::CalcTextSize("012345678901234").x;
    float inputWidth = (contentSize - charSize - 60);

    ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
    ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, inputWidth);
    ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthFixed, inputWidth);
    ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthFixed, inputWidth);
    ImGui::TableHeadersRow();

    if (ImGuiHelpers::TableInputFloat3(prop.get_name().to_string(), &data[0], inputWidth, false, -100.f, 100.f, 0.1f)) {
      modified = true;
    }

    return modified;
  }
} // namespace GUI

namespace {

  bool DrawOptionsListButton(std::string const& windowName, rttr::variant& component, rttr::type const& compType) {
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
        DrawOptionButton("Clear", component, compType);
        if (windowName != "Tag") {
          openMainWindow = DrawOptionButton("Remove Component", component, compType);
        }

        ImGui::EndTable();
      }

      ImGui::EndPopup();
    }

    return openMainWindow;
  }

  bool DrawOptionButton(std::string const& name, rttr::variant& component, rttr::type const& compType) {
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
      ECS::Entity ent{ GUI::GUIManager::GetSelectedEntity().GetRawEnttEntityID() };

      if (name == "Remove Component") {
        Reflection::ObjectFactory::GetInstance().RemoveComponentFromEntity(ent, compType);
        sEntityComponentMap[compType] = false;
        SetIsComponentEdited(true);

        // if its a prefab instance, add to overrides
        if (ent.HasComponent<Component::PrefabOverrides>()) {
          ent.GetComponent<Component::PrefabOverrides>().AddComponentRemoval(compType);
        }
        openMainWindow = false;
      }

      else if (name == "Clear") {
        // invoke the component's clear function
        compType.invoke("Clear", component, {});
        SetIsComponentEdited(true);

        // if its a prefab instance, add to overrides
        if (ent.HasComponent<Component::PrefabOverrides>()) {
          ent.GetComponent<Component::PrefabOverrides>().AddComponentModification(component);
        }
      }

      ImGui::CloseCurrentPopup();
    }

    return openMainWindow;
  }

  void ResetComponentMap(std::vector<rttr::variant> const& entityComponents) {
    for (auto&[comp, flag] : sEntityComponentMap) { flag = false; }

    for (rttr::variant const& comp : entityComponents) {
      rttr::type raw{ comp.get_type().get_wrapped_type().get_raw_type() };
      sEntityComponentMap[raw] = true;
    }
  }
}
