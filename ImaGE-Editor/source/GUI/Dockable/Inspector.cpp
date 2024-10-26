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

#include "Physics/PhysicsSystem.h"
#include <functional>
#include <Reflection/ComponentTypes.h>
#include <Events/EventManager.h>
#include <Graphics/MeshFactory.h>
#include <Graphics/Mesh.h>
#include "Asset/IGEAssets.h"
#include <Core/Systems/LayerSystem/LayerSystem.h>


namespace {
  bool InputDouble3(std::string propertyName, glm::dvec3& property, float fieldWidth, bool disabled = false);

  /*!*********************************************************************
  \brief
    Calculates the input width of the table row based on the current
    content region after subtracting the label
  \param padding
    The extra space to subtract
  \return
    The remaining width of the row
  ************************************************************************/
  float CalcInputWidth(float padding);
}

namespace GUI {
  Inspector::Inspector(const char* name) : GUIWindow(name),
    mComponentOpenStatusMap{}, mStyler{ GUIManager::GetStyler() }, mObjFactory{Reflection::ObjectFactory::GetInstance()},
    mPreviousEntity{}, mIsComponentEdited{ false }, mFirstEdit{ false }, mEditingPrefab{ false }, mEntityChanged{ false } {
    for (auto const& component : Reflection::gComponentTypes) {
      mComponentOpenStatusMap[component.get_name().to_string().c_str()] = true;
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
    
    if (currentEntity) {

      if (currentEntity != mPreviousEntity) {
        mPreviousEntity = currentEntity;
        mEntityChanged = true;
      }
      else
        mEntityChanged = false;

      static Component::PrefabOverrides* prefabOverride{ nullptr };
      static bool componentOverriden{ false };
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
      else {
        prefabOverride = nullptr;
      }

      // @TODO: EDIT WHEN NEW COMPONENTS (ALSO ITS OWN WINDOW FUNCTION)
      if (currentEntity.HasComponent<Component::Tag>()) {
        rttr::type const tagType{ rttr::type::get<Component::Tag>() };
        componentOverriden = prefabOverride && prefabOverride->IsComponentModified(tagType);

        if (TagComponentWindow(currentEntity, std::string(ICON_FA_TAG), componentOverriden)) {
          SetIsComponentEdited(true);
          if (prefabOverride) {
            prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::Tag>());
          }
        }
      }

      if (currentEntity.HasComponent<Component::Transform>()) {
        componentOverriden = prefabOverride && prefabOverride->IsComponentModified(rttr::type::get<Component::Transform>());
        Component::Transform& trans{ currentEntity.GetComponent<Component::Transform>() };
        glm::vec3 const oldPos{ trans.position };
        if (TransformComponentWindow(currentEntity, std::string(ICON_FA_ROTATE), componentOverriden)) {
          TransformHelpers::UpdateWorldTransform(currentEntity);  // must call this to update world transform according to changes to local
          SetIsComponentEdited(true);

          if (prefabOverride) {
            if (!componentOverriden && prefabOverride->subDataId == Prefabs::PrefabSubData::BasePrefabId) {
              // if root entity, ignore position changes
              // here, im assuming only 1 value can be modified per frame.
              // So if position wasn't modified, it means either rot or scale was
              if (oldPos == trans.position) {
                prefabOverride->AddComponentModification(trans);
              }
            }
            else {
              prefabOverride->AddComponentModification(trans);
            }
          }
        }
      }

      if (currentEntity.HasComponent<Component::BoxCollider>()) {
        rttr::type const colliderType{ rttr::type::get<Component::BoxCollider>() };
        componentOverriden = prefabOverride && prefabOverride->IsComponentModified(colliderType);

        if (ColliderComponentWindow(currentEntity, std::string(ICON_FA_BOMB), componentOverriden)) {
          SetIsComponentEdited(true);
          if (prefabOverride) {
            prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::BoxCollider>());
          }
        }
      }

      if (currentEntity.HasComponent<Component::Layer>()) {
        rttr::type const layerType{ rttr::type::get<Component::Layer>() };
        componentOverriden = prefabOverride && prefabOverride->IsComponentModified(layerType);

        if (LayerComponentWindow(currentEntity, std::string(ICON_FA_LAYER_GROUP), componentOverriden)) {
          SetIsComponentEdited(true);
          if (prefabOverride) {
            prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::Layer>());
          }
        }
      }

      if (currentEntity.HasComponent<Component::Material>()) {
        rttr::type const materialType{ rttr::type::get<Component::Material>() };
        componentOverriden = prefabOverride && prefabOverride->IsComponentModified(materialType);

        if (MaterialComponentWindow(currentEntity, std::string(ICON_FA_GEM), componentOverriden)) {
          SetIsComponentEdited(true);
          if (prefabOverride) {
            prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::Material>());
          }
        }
      }

      if (currentEntity.HasComponent<Component::Mesh>()) {
        rttr::type const meshType{ rttr::type::get<Component::Mesh>() };
        componentOverriden = prefabOverride && prefabOverride->IsComponentModified(meshType);

        if (MeshComponentWindow(currentEntity, std::string(ICON_FA_CUBE), componentOverriden)) {
          SetIsComponentEdited(true);
          if (prefabOverride) {
            prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::Mesh>());
          }
        }
      }

      if (currentEntity.HasComponent<Component::RigidBody>()) {
        rttr::type const rigidBodyType{ rttr::type::get<Component::RigidBody>() };
        componentOverriden = prefabOverride && prefabOverride->IsComponentModified(rigidBodyType);

        if (RigidBodyComponentWindow(currentEntity, std::string(ICON_FA_CAR), componentOverriden)) {
          SetIsComponentEdited(true);
          if (prefabOverride) {
            prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::RigidBody>());
          }
        }
      }

      if (currentEntity.HasComponent<Component::Script>()) {

        rttr::type const scriptType{ rttr::type::get<Component::Script>() };
     
        componentOverriden = prefabOverride && prefabOverride->IsComponentModified(scriptType);
        if (ScriptComponentWindow(currentEntity, std::string(ICON_FA_FILE_CODE), componentOverriden)) {
          SetIsComponentEdited(true);
          if (prefabOverride) {
            prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::Script>());
          }
        }
      }
      if (currentEntity.HasComponent<Component::Light>()) {

        rttr::type const lightType{ rttr::type::get<Component::Light>() };

        componentOverriden = prefabOverride && prefabOverride->IsComponentModified(lightType);
        if (LightComponentWindow(currentEntity, std::string(ICON_FA_FILE_CODE), componentOverriden)) {
          SetIsComponentEdited(true);
          if (prefabOverride) {
            prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::Light>());
          }
        }
      }

      if (currentEntity.HasComponent<Component::Text>()) {
        rttr::type const textType{ rttr::type::get<Component::Text>() };
        componentOverriden = prefabOverride && prefabOverride->IsComponentModified(textType);

        if (TextComponentWindow(currentEntity, std::string(ICON_FA_FONT), componentOverriden)) {
          SetIsComponentEdited(true);
          if (prefabOverride) {
            prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::Text>());
          }
        }
      }

      if (prefabOverride) {
        for (rttr::type const& type : prefabOverride->removedComponents) {
          DisplayRemovedComponent(type);
        }
      }
    }
    ImGui::PopFont();
    ImGui::End();

    // if edit is the first of this session, dispatch a SceneModifiedEvent
    if (!mFirstEdit && mIsComponentEdited) {
      QUEUE_EVENT(Events::SceneModifiedEvent);
      mFirstEdit = true;
    }
  }

  EVENT_CALLBACK_DEF(Inspector, HandleEvent) {
    switch (event->GetCategory()) {
    case Events::EventType::SAVE_SCENE:
      mIsComponentEdited = mFirstEdit = false;
      break;
    case Events::EventType::SCENE_STATE_CHANGE:
    {
      auto state{ CAST_TO_EVENT(Events::SceneStateChange)->mNewState };
      // if changing to another scene, reset modified flag
      if (state == Events::SceneStateChange::CHANGED) {
        mIsComponentEdited = mFirstEdit = mEditingPrefab = false;
      }
      else if (state == Events::SceneStateChange::NEW) {
        mIsComponentEdited = true;
        mFirstEdit = mEditingPrefab = false;
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

  bool Inspector::LayerComponentWindow(ECS::Entity entity, std::string const& icon, bool highlight) {
    bool const isOpen{ WindowBegin<Component::Layer>("Layer", icon, highlight) };
    bool modified{ false };
    
    if (isOpen) {
      Component::Layer& layer = entity.GetComponent<Component::Layer>();

      float const inputWidth{ CalcInputWidth(60.f) };

      ImGui::BeginTable("LayerTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth);

      NextRowTable("Assigned Layer");

      if (ImGui::BeginCombo("##LayerName", layer.name.c_str())) {
        std::shared_ptr<Systems::LayerSystem> layerSystemPtr = 
          Systems::SystemManager::GetInstance().GetSystem<Systems::LayerSystem>().lock();

        for (std::string const& layerName : layerSystemPtr->GetLayerNames()) {
          if (layerName == "") continue;
          if (ImGui::Selectable(layerName.c_str())) {
            layerSystemPtr->UpdateEntityLayer(entity, layer.name, layerName);
            layer.name = layerName;     
            modified = true;
          }
        }
        ImGui::EndCombo();
      }

      ImGui::EndTable();
    }

    WindowEnd(isOpen);
    return modified;
  }

  bool Inspector::MaterialComponentWindow(ECS::Entity entity, std::string const& icon, bool highlight) {
    bool const isOpen{ WindowBegin<Component::Material>("Material", icon, highlight) };
    bool modified{ false };

    if (isOpen) {
      Component::Material& material = entity.GetComponent<Component::Material>();

      float const inputWidth{ CalcInputWidth(60.f) };

      ImGui::BeginTable("MaterialTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth);

      NextRowTable("Material Type");

      static const std::vector<const char*> materialNames{
        "Default"
      };

      if (ImGui::BeginCombo("##MaterialSelection", materialNames[0])) {
        for (unsigned i{}; i < materialNames.size(); ++i) {
          if (ImGui::Selectable(materialNames[i])) {
            if (i + 1 != material.matIdx) {
              modified = true;
              material.matIdx = i + 1;
            }
            break;
          }
        }

        ImGui::EndCombo();
      }

      ImGui::EndTable();
    }

    WindowEnd(isOpen);
    return modified;
  }

  bool Inspector::ScriptComponentWindow(ECS::Entity entity, std::string const& icon, bool highlight) {
    bool const isOpen{ WindowBegin<Component::Script>("Script", icon, highlight) };
    bool modified{ false };

    if (isOpen) {
      Mono::ScriptManager* sm = &Mono::ScriptManager::GetInstance();
      ImGuiStyle& style = ImGui::GetStyle();
      std::vector <std::string> toDeleteList{};
      float const charSize = ImGui::CalcTextSize("012345678901234").x;
      float const inputWidth = (ImGui::GetWindowSize().x - charSize - 30.f) / 3.f;
      Component::Script* allScripts = &entity.GetComponent<Component::Script>();
      for (Mono::ScriptInstance& s : allScripts->mScriptList)
      {
        s.GetAllUpdatedFields();
        ImGui::Separator();
        ImGui::BeginTable("##", 2, ImGuiTableFlags_BordersInnerV);
        ImGui::TableSetupColumn("Col1", ImGuiTableColumnFlags_WidthFixed, charSize);

        ImGui::TableNextRow();
        ImGui::BeginDisabled(false);
        ImGui::TableNextColumn();
        ImGui::Text("Script");
        ImGui::TableNextColumn();
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4 originalColor = style.Colors[ImGuiCol_FrameBg];
        ImVec4 originalHColor = style.Colors[ImGuiCol_FrameBgHovered];
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.28f, 0.66f, 1.0f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.28f, 0.48f, 0.86f, 1.0f);
        if (ImGui::BeginCombo("", s.mScriptName.c_str()))
        {
          for (const std::string& sn : sm->mAllScriptNames)
          {
            auto it = std::find_if(allScripts->mScriptList.begin(), allScripts->mScriptList.end(), [sn](const Mono::ScriptInstance pair) { return pair.mScriptName == sn; });
            if (it == allScripts->mScriptList.end())
            {
              bool is_selected = (s.mScriptName.c_str() == sn);
              if (ImGui::Selectable(sn.c_str(), is_selected))
              {
                if (sn != s.mScriptName) {
                  uint32_t id{ entity.GetEntityID() };
                  std::vector<void*> arg{ &(id) };
                  s = Mono::ScriptInstance(sn, arg);
                }
              }
              if (is_selected)
              {
                ImGui::SetItemDefaultFocus();
              }
            }
          }
          ImGui::EndCombo();
        }
        ImGui::SameLine();
        ImVec4 boriginalColor = style.Colors[ImGuiCol_Button];
        ImVec4 boriginalHColor = style.Colors[ImGuiCol_ButtonHovered];
        ImVec4 boriginalAColor = style.Colors[ImGuiCol_ButtonActive];
        style.Colors[ImGuiCol_Button] = ImVec4(0.6f, 0.f, 0.29f, 1.0f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.8f, 0.1f, 0.49f, 1.0f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.7f, 0.3f, 0.39f, 1.0f);
        if (ImGui::Button("Delete"))
        {
          toDeleteList.push_back(s.mScriptName);
        }
        style.Colors[ImGuiCol_Button] = boriginalColor;
        style.Colors[ImGuiCol_ButtonHovered] = boriginalHColor;
        style.Colors[ImGuiCol_ButtonActive] = boriginalAColor;
        ImGui::EndDisabled();
        style.Colors[ImGuiCol_FrameBg] = originalColor;
        style.Colors[ImGuiCol_FrameBgHovered] = originalHColor;

        for (rttr::variant& f : s.mScriptFieldInstList)
        {
          rttr::type dataType{ f.get_type() };
          // get underlying type if it's wrapped in a pointer
          if (dataType == rttr::type::get<Mono::ScriptFieldInstance<int>>())
          {
            ImGui::TableNextRow();
            Mono::ScriptFieldInstance<int>& sfi = f.get_value<Mono::ScriptFieldInstance<int>>();
            ImGui::TableNextColumn();
            ImGui::Text(sfi.mScriptField.mFieldName.c_str());
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(ImGui::GetWindowSize().x);
            if (ImGui::InputInt(("##" + sfi.mScriptField.mFieldName).c_str(), &(sfi.mData), 0, 0, 0)) { s.SetFieldValue<int>(sfi.mData, sfi.mScriptField.mClassField); }
          }
          else if (dataType == rttr::type::get<Mono::ScriptFieldInstance<float>>())
          {
            ImGui::TableNextRow();
            Mono::ScriptFieldInstance<float>& sfi = f.get_value<Mono::ScriptFieldInstance<float>>();
            ImGui::TableNextColumn();
            ImGui::Text(sfi.mScriptField.mFieldName.c_str());
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(ImGui::GetWindowSize().x);
            if (ImGui::InputFloat(("##" + sfi.mScriptField.mFieldName).c_str(), &(sfi.mData), 0, 0, 0)) { s.SetFieldValue<float>(sfi.mData, sfi.mScriptField.mClassField); }
          }
          else if (dataType == rttr::type::get<Mono::ScriptFieldInstance<double>>())
          {
            ImGui::TableNextRow();
            Mono::ScriptFieldInstance<double>& sfi = f.get_value<Mono::ScriptFieldInstance<double>>();
            ImGui::TableNextColumn();
            ImGui::Text(sfi.mScriptField.mFieldName.c_str());
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(ImGui::GetWindowSize().x);
            if (ImGui::InputDouble(("##" + sfi.mScriptField.mFieldName).c_str(), &(sfi.mData), 0, 0, 0)) { s.SetFieldValue<double>(sfi.mData, sfi.mScriptField.mClassField); }
          }
          else if (dataType == rttr::type::get<Mono::ScriptFieldInstance<glm::dvec3>>())
          {
            ImGui::TableNextRow();
            Mono::ScriptFieldInstance<glm::dvec3>& sfi = f.get_value<Mono::ScriptFieldInstance<glm::dvec3>>();
            if (InputDouble3(("## " + sfi.mScriptField.mFieldName).c_str(), sfi.mData, inputWidth)) { s.SetFieldValue<glm::dvec3>(sfi.mData, sfi.mScriptField.mClassField); };
          }




        }


        // Check if the mouse is over the second table and the right mouse button is clicked
        ImGui::EndTable();
        ImGui::Separator();

      }




      ImVec4 originalColor = style.Colors[ImGuiCol_Button];
      ImVec4 originalHColor = style.Colors[ImGuiCol_ButtonHovered];
      style.Colors[ImGuiCol_Button] = ImVec4(0.18f, 0.28f, 0.66f, 1.0f);
      style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.48f, 0.86f, 1.0f);
      if (ImGui::Button("Add Script", ImVec2(ImGui::GetWindowSize().x, 0.0f))) {
        for (const std::string& sn : sm->mAllScriptNames)
        {
          auto it = std::find_if(allScripts->mScriptList.begin(), allScripts->mScriptList.end(), [sn](const Mono::ScriptInstance pair) { return pair.mScriptName == sn; });;
          if (it == allScripts->mScriptList.end())
          {
            uint32_t id{ entity.GetEntityID() };
            std::vector<void*> arg{ &(id) };
            allScripts->mScriptList.emplace_back(sn, arg);
            break;
          }
        }
      }
      style.Colors[ImGuiCol_Button] = originalColor;
      style.Colors[ImGuiCol_ButtonHovered] = originalHColor;

      for (const std::string& tds : toDeleteList)
      {
        auto it = std::find_if(allScripts->mScriptList.begin(), allScripts->mScriptList.end(), [tds](const Mono::ScriptInstance pair) { return pair.mScriptName == tds; });
        allScripts->mScriptList.erase(it);
      }
    }

    WindowEnd(isOpen);
    return modified;
  }



  bool Inspector::TagComponentWindow(ECS::Entity entity, std::string const& icon, bool highlight) {
    bool const isOpen{ WindowBegin<Component::Tag>("Tag", icon, highlight) };
    bool modified{ false };

    if (isOpen) {
      std::string tag{ entity.GetTag() };
      ImGui::PushFont(mStyler.GetCustomFont(GUI::MONTSERRAT_SEMIBOLD));
      ImGui::SetNextItemWidth(INPUT_SIZE);
      if (ImGui::InputText("##Tag", &tag, ImGuiInputTextFlags_EnterReturnsTrue)) {
        entity.SetTag(tag);
        modified = true;
      }
      ImGui::PopFont();

      DrawAddButton();
    }

    WindowEnd(isOpen);
    return modified;
  }

  bool Inspector::TextComponentWindow(ECS::Entity entity, std::string const& icon, bool highlight) {
    bool const isOpen{ WindowBegin<Component::Text>("Text", icon, highlight) };
    bool modified{ false };

    if (isOpen) {
      auto& text = entity.GetComponent<Component::Text>();

      // @TODO: TEMP, TO BE REPLACED WITH ACTUAL FONTS
      std::vector<const char*> availableFonts{ "Arial", "Test1", "Test2" };

      float const inputWidth{ CalcInputWidth(60.f) };

      ImGui::BeginTable("TextTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      
      NextRowTable("Font Family");
      if (ImGui::BeginCombo("##TextName", text.fontName.c_str())) {
        for (const char* fontName : availableFonts) {
          if (ImGui::Selectable(fontName)) {
            text.fontName = fontName;
            modified = true;
          }
        }
        ImGui::EndCombo();
      }
      
      NextRowTable("Color");
      if (ImGui::ColorEdit4("##TextColor", &text.color[0])) {
        modified = true;
      }

      NextRowTable("Text Input");
      if (ImGui::InputTextMultiline("##TextInput", &text.textContent)) {
        modified = true;
      }

      NextRowTable("Scale");
      if (ImGui::DragFloat("##TextScale", &text.scale, .001f, 0.f, 5.f)) {
        modified = true;
      }

      ImGui::EndTable();
    }

    WindowEnd(isOpen);
    return modified;
  }

  bool Inspector::TransformComponentWindow(ECS::Entity entity, std::string const& icon, bool highlight) {
    bool const isOpen{ WindowBegin<Component::Transform>("Transform", icon, highlight) };
    bool modified{ false };

    if (isOpen) {
      Component::Transform& transform = entity.GetComponent<Component::Transform>();

      float const inputWidth{ CalcInputWidth(50.f) / 3.f };

      ImGui::BeginTable("LocalTransformTable", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableHeadersRow();

      // @TODO: Replace min and max with the world min and max
      if (ImGuiHelpers::TableInputFloat3("Position", &transform.position[0], inputWidth, false, -100.f, 100.f, 0.1f)) {
        modified = true;
      }
      glm::vec3 localRot{ transform.eulerAngles };
      if (ImGuiHelpers::TableInputFloat3("Rotation", &localRot[0], inputWidth, false, -360.f, 360.f, 0.3f)) {
        transform.SetLocalRotWithEuler(localRot);
        modified = true;
      }
      if (ImGuiHelpers::TableInputFloat3("Scale", &transform.scale[0], inputWidth, false, 0.001f, 100.f, 0.3f)) {
        modified = true;
      }
      ImGui::EndTable();

      ImGui::BeginTable("WorldTransformTable", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableHeadersRow();

      // only allow local transform to be modified
      glm::vec3 worldRot{ transform.GetWorldEulerAngles() };
      ImGui::BeginDisabled();
      ImGuiHelpers::TableInputFloat3("World Position", &transform.worldPos[0], inputWidth, false, -100.f, 100.f, 0.1f);
      ImGuiHelpers::TableInputFloat3("World Rotation", &worldRot[0], inputWidth, false, 0.f, 360.f, 0.1f);
      ImGuiHelpers::TableInputFloat3("World Scale", &transform.worldScale[0], inputWidth, false, 0.001f, 100.f, 1.f);
      ImGui::EndDisabled();

      ImGui::EndTable();
    }

    WindowEnd(isOpen);
    return modified;
  }

  bool Inspector::MeshComponentWindow(ECS::Entity entity, std::string const& icon, bool highlight) {
    bool const isOpen{ WindowBegin<Component::Mesh>("Mesh", icon, highlight) };
    bool modified{ false };

    if (isOpen) {
      Component::Mesh& mesh{ entity.GetComponent<Component::Mesh>() };
      static const std::vector<const char*> meshNames{
        "None", "Cube", "Plane"
      };

      float const inputWidth{ CalcInputWidth(60.f) };

      ImGui::BeginTable("MeshTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth);

      NextRowTable("Mesh Type");

      if (ImGui::BeginCombo("##MeshSelection", mesh.meshName.c_str())) {
        for (unsigned i{}; i < meshNames.size(); ++i) {
          const char* selected{ meshNames[i] };
          if (ImGui::Selectable(selected)) {
            if (i != 0) {

              //mesh.mesh = std::make_shared<Graphics::Mesh>(Graphics::MeshFactory::CreateModelFromString(selected));
                mesh.meshSource = IGE_ASSETMGR.LoadRef<IGE::Assets::MeshAsset>(selected);
            }

            if (selected != mesh.meshName) {
              modified = true;
              mesh.meshName = selected;
            }
            break;
          }
        }

        ImGui::EndCombo();
      }
      // allow dropping of models
      else if (ImGui::BeginDragDropTarget())
      {
        ImGuiPayload const* drop = ImGui::AcceptDragDropPayload(AssetPayload::sAssetDragDropPayload);
        if (drop) {
          AssetPayload assetPayload{ reinterpret_cast<const char*>(drop->Data) };
          if (assetPayload.mAssetType == AssetPayload::MODEL) {
            //auto meshSrc{ std::make_shared<Graphics::Mesh>(Graphics::MeshFactory::CreateModelFromImport(assetPayload.GetFilePath())) };
            mesh.meshSource = IGE_ASSETMGR.LoadRef<IGE::Assets::MeshAsset>(assetPayload.GetFilePath());
            mesh.meshName = assetPayload.GetFileName();
            modified = true;
          }
        }
        ImGui::EndDragDropTarget();
      }

      NextRowTable("Cast Shadows");
      if (ImGui::Checkbox("##CastShadows", &mesh.castShadows)) {
        modified = true;
      }

      ImGui::BeginDisabled();
      NextRowTable("Receive Shadows");
      ImGui::Checkbox("##CastShadows", &mesh.receiveShadows);
      ImGui::EndDisabled();

      ImGui::EndTable();
    }

    WindowEnd(isOpen);
    return modified;
  }

  bool Inspector::RigidBodyComponentWindow(ECS::Entity entity, std::string const& icon, bool highlight) {
    bool const isOpen{ WindowBegin<Component::RigidBody>("RigidBody", icon, highlight) };
    bool modified{ false };

    if (isOpen) {
      // Assuming 'rigidBody' is an instance of RigidBody
      Component::RigidBody& rigidBody{ entity.GetComponent<Component::RigidBody>() };

      float const inputWidth{ CalcInputWidth(50.f) / 3.f };

      ImGui::BeginTable("RigidBodyTable", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableHeadersRow();

      if (ImGuiHelpers::TableInputFloat3("Velocity", &rigidBody.velocity.x, inputWidth, false, -100.f, 100.f, 0.1f)) {
        IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::VELOCITY);
        modified = true;
      }
      if (ImGuiHelpers::TableInputFloat3("Angular Velocity", &rigidBody.angularVelocity.x, inputWidth, false, -100.f, 100.f, 0.1f)) {
        IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::ANGULAR_VELOCITY);
        modified = true;
      }

      ImGui::EndTable();

      ImGui::BeginTable("ShapeSelectionTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth * 3);

      NextRowTable("Static Friction");
      if (ImGui::DragFloat("##RigidBodyStaticFriction", &rigidBody.staticFriction, 0.01f, 0.0f, 1.0f)) {
        IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::STATIC_FRICTION);
        SetIsComponentEdited(true);
      }

      NextRowTable("Dynamic Friction");
      if (ImGui::DragFloat("##RigidBodyDynamicFriction", &rigidBody.dynamicFriction, 0.01f, 0.0f, 1.0f)) {
          IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::STATIC_FRICTION);
          SetIsComponentEdited(true);
      }

      NextRowTable("Restitution");
      if (ImGui::DragFloat("##RigidBodyRestitution", &rigidBody.restitution, 0.01f, 0.0f, 1.0f)) {
        IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::RESTITUTION);
        modified = true;
      }

      NextRowTable("Gravity Factor");
      if (ImGui::DragFloat("##RigidBodyGravityFactor", &rigidBody.gravityFactor, 0.01f, -10.0f, 10.0f)) {
        IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::GRAVITY_FACTOR);
        modified = true;
      }

      NextRowTable("Mass");
      if (ImGui::DragFloat("##RigidBodyMass", &rigidBody.mass, 0.01f, 0.0f, 1000'000.0f)) {
          IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::MASS);
          SetIsComponentEdited(true);
      }

      // Motion Type Selection
      static const char* motionTypes[] = { "Dynamic", "Kinematic" };
      int currentMotionType = static_cast<int>(rigidBody.motionType);

      NextRowTable("Motion Type");
      if (ImGui::Combo("##Motion Type", &currentMotionType, motionTypes, IM_ARRAYSIZE(motionTypes))) {
        rigidBody.motionType = static_cast<Component::RigidBody::MotionType>(currentMotionType);
        IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::MOTION);
        modified = true;
      }

      ImGui::EndTable();
    }

    WindowEnd(isOpen);
    return modified;
  }

  bool Inspector::ColliderComponentWindow(ECS::Entity entity, std::string const& icon, bool highlight) {
    bool const isOpen{ WindowBegin<Component::BoxCollider>("Collider", icon, highlight) };
    bool modified{ false };

    if (isOpen) {
      // Assuming 'collider' is an instance of Collider
      Component::BoxCollider& collider{ entity.GetComponent<Component::BoxCollider>() };
      float const inputWidth{ CalcInputWidth(50.f) / 3.f };

      ImGui::BeginTable("ColliderTable", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableHeadersRow();

      if (ImGuiHelpers::TableInputFloat3("Scale", &collider.scale.x, inputWidth, false, 0.f, 100.f, 1.f)) {
        SetIsComponentEdited(true);
      }
      if (ImGuiHelpers::TableInputFloat3("Position Offset", &collider.positionOffset.x, inputWidth, false, -100.f, 100.f, 0.1f)) {
        SetIsComponentEdited(true);
      }
      //if (ImGuiHelpers::TableInputFloat3("Rotation Offset", &collider.rotationOffset.x, inputWidth, false, 0.f, 360.f, 0.1f)) {
      //  SetIsComponentEdited(true);
      //}

      ImGui::EndTable();

      // Shape Type Selection
      //const char* shapeTypes[] = { "Unknown", "Sphere", "Capsule", "Box", "Triangle", "ConvexHull", "Mesh", "HeightField", "Compound" };
      //int currentShapeType = static_cast<int>(collider.type);

      //ImGui::BeginTable("ShapeSelectionTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      //ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      //ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth * 3);

      //ImGui::TableNextRow();
      //ImGui::TableSetColumnIndex(0);
      //ImGui::Text("Shape Type");
      //ImGui::TableSetColumnIndex(1);
      //ImGui::SetNextItemWidth(INPUT_SIZE);
      //if (ImGui::Combo("##ShapeType", &currentShapeType, shapeTypes, IM_ARRAYSIZE(shapeTypes))) {
      //  collider.type = static_cast<JPH::EShapeSubType>(currentShapeType);
      //  SetIsComponentEdited(true);
      //}

      //ImGui::EndTable();
    }

    WindowEnd(isOpen);
    return modified;
  }

  bool Inspector::LightComponentWindow(ECS::Entity entity, std::string const& icon, bool highlight) {
    bool const isOpen{ WindowBegin<Component::Light>("Light", icon, highlight) };
    bool modified{ false };

    if (isOpen) {
      const std::vector<std::string> Lights{ "Directional","Spotlight" };
    //  // Assuming 'collider' is an instance of Collider
      Component::Light& light{ entity.GetComponent<Component::Light>() };
      float contentSize = ImGui::GetContentRegionAvail().x;
      float charSize = ImGui::CalcTextSize("012345678901234").x;
      float inputWidth = (contentSize - charSize - 50) / 3;

      ImGui::BeginTable("##", 2, ImGuiTableFlags_BordersInnerV);
      ImGui::TableSetupColumn("Col1", ImGuiTableColumnFlags_WidthFixed, charSize);

      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::Text("Type");
      ImGui::TableNextColumn();

      if (ImGui::BeginCombo("", Lights[light.mType].c_str()))
      {
        for (int s{ 0 }; s < static_cast<int>(Component::LIGHT_COUNT); ++s)
        {
          if (Lights[s] != Lights[light.mType])
          {
            bool is_selected = (Lights[light.mType] == Lights[s]);
            if (ImGui::Selectable(Lights[s].c_str(), is_selected))
            {
              if (Lights[s] != Lights[light.mType]) {
                light.mType = static_cast<Component::Light_Type>(s);
              }
            }
            if (is_selected)
            {
              ImGui::SetItemDefaultFocus();
            }
          }
        }
        ImGui::EndCombo();
      }
      ImGui::EndTable();


      ImGui::BeginTable("ColliderTable", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableHeadersRow();

      if (ImGuiHelpers::TableInputFloat3("Position", &light.mPosition[0], inputWidth, false, -100.f, 100.f, 0.1f)) {
        modified = true;
      }
   
      if (ImGuiHelpers::TableInputFloat3("Direction", &light.mDirection[0], inputWidth, false, -100.f, 100.f, 0.1f)) {
        modified = true;
      }
      if (ImGuiHelpers::TableInputFloat3("Color", &light.mColor[0], inputWidth, false, -100.f, 100.f, 0.1f)) {
        modified = true;
      }
    
    ImGui::EndTable();

    }

    WindowEnd(isOpen);
    return modified;
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
        
        // @TODO: EDIT WHEN NEW COMPONENTS
        DrawAddComponentButton<Component::BoxCollider>("Collider", ICON_FA_BOMB);
        DrawAddComponentButton<Component::Layer>("Layer", ICON_FA_LAYER_GROUP);
        DrawAddComponentButton<Component::Material>("Material", ICON_FA_GEM);
        // @TODO: Temporarily forcing material to be added with mesh
        DrawAddComponentButton<Component::Mesh>("Mesh", ICON_FA_CUBE);
        DrawAddComponentButton<Component::RigidBody>("RigidBody", ICON_FA_CAR);
        DrawAddComponentButton<Component::Script>("Script", ICON_FA_FILE_CODE);
        DrawAddComponentButton<Component::Tag>("Tag", ICON_FA_TAG);
        DrawAddComponentButton<Component::Text>("Text", ICON_FA_FONT);
        DrawAddComponentButton<Component::Transform>("Transform", std::string(ICON_FA_ROTATE));
        DrawAddComponentButton<Component::Light>("Light", std::string(ICON_FA_ROTATE));

        ImGui::EndTable();
      }

      ImGui::EndPopup();
    }
  }

  void Inspector::WindowEnd(bool const isOpen) {
    if (isOpen) {
      ImGui::TreePop();
      ImGui::PopFont();
    }

    ImGui::Separator();
  }

  void Inspector::NextRowTable(const char* labelName) const {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(labelName);
    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(INPUT_SIZE);
  }
 
} // namespace GUI

namespace {
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

  float CalcInputWidth(float padding) {
    static float const charSize = ImGui::CalcTextSize("012345678901234").x;
    return ImGui::GetContentRegionAvail().x - charSize - padding;
  }
}
