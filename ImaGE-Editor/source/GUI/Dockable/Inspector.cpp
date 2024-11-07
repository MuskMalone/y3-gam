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
#include <typeindex>
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
#

#define ICON_PADDING "   "

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
    mComponentOpenStatusMap{}, mStyler{ GUIManager::GetStyler() }, 
    mComponentIcons{
      { typeid(Component::AudioListener), ICON_FA_EAR_LISTEN ICON_PADDING},
      { typeid(Component::AudioSource), ICON_FA_VOLUME_HIGH ICON_PADDING},
      { typeid(Component::Tag), ICON_FA_TAG ICON_PADDING },
      { typeid(Component::Transform), ICON_FA_ROTATE ICON_PADDING },
      { typeid(Component::BoxCollider), ICON_FA_BOMB ICON_PADDING },
      { typeid(Component::SphereCollider), ICON_FA_CIRCLE ICON_PADDING },
      { typeid(Component::CapsuleCollider), ICON_FA_CAPSULES ICON_PADDING },
      { typeid(Component::Layer), ICON_FA_LAYER_GROUP ICON_PADDING },
      { typeid(Component::MaterialComponent), ICON_FA_GEM ICON_PADDING },
      { typeid(Component::Mesh), ICON_FA_CUBE ICON_PADDING },
      { typeid(Component::RigidBody), ICON_FA_CAR ICON_PADDING },
      { typeid(Component::Script), ICON_FA_FILE_CODE ICON_PADDING },
      { typeid(Component::Text), ICON_FA_FONT ICON_PADDING },
      { typeid(Component::Light), ICON_FA_LIGHTBULB ICON_PADDING },
      { typeid(Component::Canvas), ICON_FA_PAINTBRUSH},
      { typeid(Component::Image), ICON_FA_IMAGE},
      { typeid(Component::Camera), ICON_FA_CAMERA}
    },
    mObjFactory{Reflection::ObjectFactory::GetInstance()},
    mPreviousEntity{}, mIsComponentEdited{ false }, mFirstEdit{ false }, mEditingPrefab{ false }, mEntityChanged{ false } {
    for (auto const& component : Reflection::gComponentTypes) {
      mComponentOpenStatusMap[component.get_name().to_string().c_str()] = true;
    }

    // get notified when scene is saved
    SUBSCRIBE_CLASS_FUNC(Events::EventType::SAVE_SCENE, &Inspector::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::SCENE_STATE_CHANGE, &Inspector::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::EDIT_PREFAB, &Inspector::HandleEvent, this);

    // simple check to ensure all components have icons
    if (Reflection::gComponentTypes.size() != mComponentIcons.size()) {
      throw Debug::Exception<Inspector>(Debug::LVL_CRITICAL, Msg("sComponentIcons and gComponentTypes size mismatch! Did you forget to add an icon?"));
    }
  }

  void Inspector::Run() {
    ImGuiStyle& style = ImGui::GetStyle();
    float oldItemSpacingX = style.ItemSpacing.x;
    float oldCellPaddingX = style.CellPadding.x;
    style.ItemSpacing.x = ITEM_SPACING;
    style.CellPadding.x = CELL_PADDING;

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
        std::string const& pfbName{ IGE_ASSETMGR.GetAsset<IGE::Assets::PrefabAsset>(prefabOverride->guid)->mPrefabData.mName };
        ImGui::PushFont(mStyler.GetCustomFont(GUI::MONTSERRAT_REGULAR));
        ImGui::Text("Prefab instance of");
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, sComponentHighlightCol);
        ImGui::Text(pfbName.c_str());
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

        if (TagComponentWindow(currentEntity, componentOverriden)) {
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
        if (TransformComponentWindow(currentEntity, componentOverriden)) {
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

        if (BoxColliderComponentWindow(currentEntity, componentOverriden)) {
          SetIsComponentEdited(true);
          if (prefabOverride) {
            prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::BoxCollider>());
          }
        }
      }

      if (currentEntity.HasComponent<Component::SphereCollider>()) {
          rttr::type const colliderType{ rttr::type::get<Component::SphereCollider>() };
          componentOverriden = prefabOverride && prefabOverride->IsComponentModified(colliderType);

          if (SphereColliderComponentWindow(currentEntity, componentOverriden)) {
              SetIsComponentEdited(true);
              if (prefabOverride) {
                  prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::SphereCollider>());
              }
          }
      }

      if (currentEntity.HasComponent<Component::CapsuleCollider>()) {
          rttr::type const colliderType{ rttr::type::get<Component::CapsuleCollider>() };
          componentOverriden = prefabOverride && prefabOverride->IsComponentModified(colliderType);

          if (CapsuleColliderComponentWindow(currentEntity, componentOverriden)) {
              SetIsComponentEdited(true);
              if (prefabOverride) {
                  prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::CapsuleCollider>());
              }
          }
      }

      // don't run in PrefabEditor since layers are tied to a scene
      if (!mEditingPrefab && currentEntity.HasComponent<Component::Layer>()) {
        rttr::type const layerType{ rttr::type::get<Component::Layer>() };
        componentOverriden = prefabOverride && prefabOverride->IsComponentModified(layerType);

        if (LayerComponentWindow(currentEntity, componentOverriden)) {
          SetIsComponentEdited(true);
          if (prefabOverride) {
            prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::Layer>());
          }
        }
      }

      if (currentEntity.HasComponent<Component::MaterialComponent>()) {
        rttr::type const materialType{ rttr::type::get<Component::MaterialComponent>() };
        componentOverriden = prefabOverride && prefabOverride->IsComponentModified(materialType);

        if (MaterialComponentWindow(currentEntity, componentOverriden)) {
          SetIsComponentEdited(true);
          if (prefabOverride) {
            prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::MaterialComponent>());
          }
        }
      }

      if (currentEntity.HasComponent<Component::Mesh>()) {
        rttr::type const meshType{ rttr::type::get<Component::Mesh>() };
        componentOverriden = prefabOverride && prefabOverride->IsComponentModified(meshType);

        if (MeshComponentWindow(currentEntity, componentOverriden)) {
          SetIsComponentEdited(true);
          if (prefabOverride) {
            prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::Mesh>());
          }
        }
      }

      if (currentEntity.HasComponent<Component::RigidBody>()) {
        rttr::type const rigidBodyType{ rttr::type::get<Component::RigidBody>() };
        componentOverriden = prefabOverride && prefabOverride->IsComponentModified(rigidBodyType);

        if (RigidBodyComponentWindow(currentEntity, componentOverriden)) {
          SetIsComponentEdited(true);
          if (prefabOverride) {
            prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::RigidBody>());
          }
        }
      }

      if (currentEntity.HasComponent<Component::Script>()) {

        rttr::type const scriptType{ rttr::type::get<Component::Script>() };
     
        componentOverriden = prefabOverride && prefabOverride->IsComponentModified(scriptType);
        if (ScriptComponentWindow(currentEntity, componentOverriden)) {
          SetIsComponentEdited(true);
          if (prefabOverride) {
            prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::Script>());
          }
        }
      }
      if (currentEntity.HasComponent<Component::Light>()) {

        rttr::type const lightType{ rttr::type::get<Component::Light>() };

        componentOverriden = prefabOverride && prefabOverride->IsComponentModified(lightType);
        if (LightComponentWindow(currentEntity, componentOverriden)) {
          SetIsComponentEdited(true);
          if (prefabOverride) {
            prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::Light>());
          }
        }
      }

      if (currentEntity.HasComponent<Component::Text>()) {
        rttr::type const textType{ rttr::type::get<Component::Text>() };
        componentOverriden = prefabOverride && prefabOverride->IsComponentModified(textType);

        if (TextComponentWindow(currentEntity, componentOverriden)) {
          SetIsComponentEdited(true);
          if (prefabOverride) {
            prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::Text>());
          }
        }
      }

      if (currentEntity.HasComponent<Component::Image>()) {
          rttr::type const imageType{ rttr::type::get<Component::Image>() };
          componentOverriden = prefabOverride && prefabOverride->IsComponentModified(imageType);

          if (ImageComponentWindow(currentEntity, componentOverriden)) {
              SetIsComponentEdited(true);
              if (prefabOverride) {
                  prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::Image>());
              }
          }
      }

      if (currentEntity.HasComponent<Component::Camera>()) {
          rttr::type const cameraType{ rttr::type::get<Component::Camera>() };
          componentOverriden = prefabOverride && prefabOverride->IsComponentModified(cameraType);

          if (CameraComponentWindow(currentEntity, componentOverriden)) {
              SetIsComponentEdited(true);
              if (prefabOverride) {
                  prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::Camera>());
              }
          }
      }

      if (currentEntity.HasComponent<Component::AudioListener>()) {
          rttr::type const listenerType{ rttr::type::get<Component::AudioListener>() };
          componentOverriden = prefabOverride && prefabOverride->IsComponentModified(listenerType);

          if (AudioListenerComponentWindow(currentEntity, componentOverriden)) {
              SetIsComponentEdited(true);
              if (prefabOverride) {
                  prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::AudioListener>());
              }
          }
      }
      if (currentEntity.HasComponent<Component::AudioSource>()) {
          rttr::type const sourceType{ rttr::type::get<Component::AudioSource>() };
          componentOverriden = prefabOverride && prefabOverride->IsComponentModified(sourceType);

          if (AudioSourceComponentWindow(currentEntity, componentOverriden)) {
              SetIsComponentEdited(true);
              if (prefabOverride) {
                  prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::AudioSource>());
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
    style.ItemSpacing.x = oldItemSpacingX;
    style.CellPadding.x = oldCellPaddingX;
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

  bool Inspector::LayerComponentWindow(ECS::Entity entity, bool highlight) {
    bool const isOpen{ WindowBegin<Component::Layer>("Layer", highlight) };
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
            entity.SetLayer(layerName);
            //layer.name = layerName;     
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

  bool Inspector::MaterialComponentWindow(ECS::Entity entity, bool highlight) {
    bool const isOpen{ WindowBegin<Component::MaterialComponent>("Material", highlight) };
    bool modified{ false };

    if (isOpen) {
      Component::MaterialComponent& material = entity.GetComponent<Component::MaterialComponent>();

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
            if (i != material.matIdx) {
              modified = true;
              material.matIdx = i;
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

  bool Inspector::ScriptComponentWindow(ECS::Entity entity, bool highlight) {
    bool const isOpen{ WindowBegin<Component::Script>("Script", highlight) };
    bool modified{ false };

    if (isOpen) {
      Mono::ScriptManager* sm = &Mono::ScriptManager::GetInstance();
      std::vector <std::string> toDeleteList{};
      static std::string selectedScript{};
      float const inputWidth{ CalcInputWidth(60.f) };
      Component::Script* allScripts = &entity.GetComponent<Component::Script>();
      for (Mono::ScriptInstance& s : allScripts->mScriptList)
      {
        s.GetAllUpdatedFields();
        ImGui::Separator();

        {
          ImGuiStyle& style = ImGui::GetStyle();
          float const deleteBtnPos{ ImGui::GetCursorPosX() + FIRST_COLUMN_LENGTH };
          ImGui::PushFont(mStyler.GetCustomFont(GUI::MONTSERRAT_REGULAR));
          ImGui::Text(s.mScriptName.c_str());
          ImGui::PopFont();
          ImGui::SameLine();
          ImVec4 const boriginalColor = style.Colors[ImGuiCol_Button];
          ImVec4 const boriginalHColor = style.Colors[ImGuiCol_ButtonHovered];
          ImVec4 const boriginalAColor = style.Colors[ImGuiCol_ButtonActive];
          style.Colors[ImGuiCol_Button] = ImVec4(0.6f, 0.f, 0.29f, 1.0f);
          style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.8f, 0.1f, 0.49f, 1.0f);
          style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.7f, 0.3f, 0.39f, 1.0f);
          ImGui::SetCursorPosX(deleteBtnPos);
          if (ImGui::Button(("Delete##" + s.mScriptName).c_str()))
          {
            toDeleteList.push_back(s.mScriptName);

            // if selection is empty, set it to the deleted script
            if (selectedScript.empty()) {
              selectedScript = s.mScriptName;
            }
          }
          style.Colors[ImGuiCol_Button] = boriginalColor;
          style.Colors[ImGuiCol_ButtonHovered] = boriginalHColor;
          style.Colors[ImGuiCol_ButtonActive] = boriginalAColor;
        }

        ImGui::BeginTable("##", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);
        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
        ImGui::TableSetupColumn("Col1", ImGuiTableColumnFlags_WidthFixed, inputWidth);
        bool isPrevVec3{ false };
        for (rttr::variant& f : s.mScriptFieldInstList)
        {
          bool const isCurrVec3{ f.is_type<Mono::DataMemberInstance<Mono::DataMemberInstance<glm::dvec3>>>()
            || f.is_type<Mono::DataMemberInstance<Mono::DataMemberInstance<glm::vec3>>>() };
          // if there is a current vec3 table and we don't need it, end it
          if (isPrevVec3 && !isCurrVec3) {
            EndVec3Table();
            ImGui::BeginTable("##", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);
            ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
            ImGui::TableSetupColumn("Col1", ImGuiTableColumnFlags_WidthFixed, inputWidth);
          }

          if (f.is_type<Mono::DataMemberInstance<int>>())
          {
            Mono::DataMemberInstance<int>& sfi = f.get_value<Mono::DataMemberInstance<int>>();
            NextRowTable(sfi.mScriptField.mFieldName.c_str());
            if (ImGui::DragInt(("##" + sfi.mScriptField.mFieldName).c_str(), &(sfi.mData), 1)) {
              s.SetFieldValue<int>(sfi.mData, sfi.mScriptField.mClassField);
              modified = true;
            }
          }
          else if (f.is_type<Mono::DataMemberInstance<float>>())
          {
            Mono::DataMemberInstance<float>& sfi = f.get_value<Mono::DataMemberInstance<float>>();
            NextRowTable(sfi.mScriptField.mFieldName.c_str());
            if (ImGui::DragFloat(("##" + sfi.mScriptField.mFieldName).c_str(), &(sfi.mData), 0.1f)) {
              s.SetFieldValue<float>(sfi.mData, sfi.mScriptField.mClassField);
              modified = true;
            }
          }
          else if (f.is_type<Mono::DataMemberInstance<double>>())
          {
            Mono::DataMemberInstance<double>& sfi = f.get_value<Mono::DataMemberInstance<double>>();
            NextRowTable(sfi.mScriptField.mFieldName.c_str());
            if (ImGui::DragScalar(("##" + sfi.mScriptField.mFieldName).c_str(), ImGuiDataType_Double, &sfi.mData, 0.1f, "%.3f")) {
              s.SetFieldValue<double>(sfi.mData, sfi.mScriptField.mClassField);
              modified = true;
            }
          }
          else if (f.is_type<Mono::DataMemberInstance<glm::vec3>>())
          {
            // if prev element wasnt a vec3, end it and start a new table
            if (!isPrevVec3) {
              ImGui::EndTable();
              BeginVec3Table("ScriptVec3Table", inputWidth / 3.f);
            }

            Mono::DataMemberInstance<glm::vec3>& sfi = f.get_value<Mono::DataMemberInstance<glm::vec3>>();
            if (ImGuiHelpers::TableInputFloat3(sfi.mScriptField.mFieldName, &sfi.mData[0], inputWidth, false, -FLT_MAX, FLT_MAX, 0.1f)) {
              s.SetFieldValue<glm::dvec3>(sfi.mData, sfi.mScriptField.mClassField);
              modified = true;
            }
          }
          else if (f.is_type<Mono::DataMemberInstance<glm::dvec3>>())
          {
            // if prev element wasnt a vec3, end it and start a new table
            if (!isPrevVec3) {
              ImGui::EndTable();
              BeginVec3Table("ScriptdVec3Table", inputWidth / 3.f);
            }

            Mono::DataMemberInstance<glm::dvec3>& sfi = f.get_value<Mono::DataMemberInstance<glm::dvec3>>();
            if (ImGuiHelpers::TableInputDouble3(sfi.mScriptField.mFieldName, sfi.mData, inputWidth, false, -DBL_MAX, DBL_MAX, 0.1f)) {
              s.SetFieldValue<glm::dvec3>(sfi.mData, sfi.mScriptField.mClassField);
              modified = true;
            }
          }
          else if (f.is_type<Mono::DataMemberInstance<Mono::ScriptInstance>>())
          {
            Mono::DataMemberInstance<Mono::ScriptInstance>& sfi = f.get_value<Mono::DataMemberInstance<Mono::ScriptInstance>>();
            if (sfi.mScriptField.mFieldType == Mono::ScriptFieldType::ENTITY || sfi.mScriptField.mFieldType == Mono::ScriptFieldType::INSIDE)
            {
              NextRowTable(sfi.mScriptField.mFieldName.c_str());

              //Set the default display value.
              ECS::Entity::EntityID currID = entt::null;
              std::string msg{ "No Entity Attached" };
              if (sfi.mData.mClassInst && ECS::EntityManager::GetInstance().IsValidEntity(static_cast<ECS::Entity::EntityID>(sfi.mData.mScriptFieldInstList[0].get_value<Mono::DataMemberInstance<unsigned>>().mData)))
              {
                msg = ECS::Entity(static_cast<ECS::Entity::EntityID>(sfi.mData.mScriptFieldInstList[0].get_value<Mono::DataMemberInstance<unsigned>>().mData)).GetTag();
                ECS::Entity::EntityID currID = static_cast<ECS::Entity::EntityID>(sfi.mData.mScriptFieldInstList[0].get_value<Mono::DataMemberInstance<unsigned>>().mData);
              }

              if (ImGui::BeginCombo("##", msg.c_str()))
              {
                for (const ECS::Entity e : ECS::EntityManager::GetInstance().GetAllEntities())
                {
                  if (e.GetRawEnttEntityID() != currID)
                  {
                    bool is_selected = (e.GetRawEnttEntityID() == currID);
                    if (ImGui::Selectable(e.GetTag().c_str(), is_selected))
                    {
                      if (e.GetRawEnttEntityID() != currID) {
                        if (!sfi.mData.mClassInst)
                        {
                          sfi.mData = Mono::ScriptInstance(sfi.mData.mScriptName);
                          sfi.mData.SetEntityID(e.GetRawEnttEntityID());
                          s.SetFieldValue<MonoObject>(sfi.mData.mClassInst, sfi.mScriptField.mClassField);
                        }
                        else
                        {
                          sfi.mData.mScriptFieldInstList[0].get_value<Mono::DataMemberInstance<unsigned>>().mData = static_cast<unsigned>(e.GetRawEnttEntityID());
                          sfi.mData.SetAllFields();
                        }
                      }
                      modified = true;
                      break;
                    }
                    if (is_selected)
                    {
                      ImGui::SetItemDefaultFocus();
                    }
                  }

                }
                ImGui::EndCombo();
              }
            }

          }

          isPrevVec3 = f.is_type<Mono::DataMemberInstance<glm::dvec3>>()
            || f.is_type<Mono::DataMemberInstance<glm::vec3>>();
        }

        // remember to end table if the last element was a vec3
        if (isPrevVec3) { EndVec3Table(); }

        // Check if the mouse is over the second table and the right mouse button is clicked
        ImGui::EndTable();
        ImGui::Separator();

      }

      {
        ImVec2 const buttonSize(100.0f, 30.0f);
        float const dropdownPos{ ImGui::GetCursorPosX() + FIRST_COLUMN_LENGTH };
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4 const originalColor = style.Colors[ImGuiCol_FrameBg];
        ImVec4 const originalHColor = style.Colors[ImGuiCol_FrameBgHovered];
        ImVec4 const originalBColor = style.Colors[ImGuiCol_Button];
        ImVec4 const originalBHColor = style.Colors[ImGuiCol_ButtonHovered];
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.28f, 0.66f, 1.0f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.28f, 0.48f, 0.86f, 1.0f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.18f, 0.28f, 0.66f, 1.0f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.48f, 0.86f, 1.0f);
        if (ImGui::Button("Add Script", buttonSize)) {
          if (!selectedScript.empty()) {
            allScripts->mScriptList.emplace_back(selectedScript);
            allScripts->mScriptList[allScripts->mScriptList.size() - 1].SetEntityID(entity.GetRawEnttEntityID());

            // refresh the displayed script in the dropdown box
            // display the next available script. If not, leave it blank
            selectedScript.clear();
            for (const std::string& sn : sm->mAllScriptNames)
            {
              auto it = std::find_if(allScripts->mScriptList.begin(), allScripts->mScriptList.end(), [sn](const Mono::ScriptInstance pair) { return pair.mScriptName == sn; });;
              if (it != allScripts->mScriptList.end()) { continue; }

              selectedScript = sn;
              break;
            }
          }
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(dropdownPos);
        ImGui::SetNextItemWidth(inputWidth);
        if (ImGui::BeginCombo("##ScriptSelection", selectedScript.empty()  ? "Select Script" : selectedScript.c_str()))
        {
          for (const std::string& sn : sm->mAllScriptNames)
          {
            auto it = std::find_if(allScripts->mScriptList.begin(), allScripts->mScriptList.end(), [sn](const Mono::ScriptInstance pair) { return pair.mScriptName == sn; });
            if (it == allScripts->mScriptList.end())
            {
              bool const is_selected = (selectedScript.c_str() == sn);
              if (ImGui::Selectable(sn.c_str(), is_selected))
              {
                selectedScript = sn;
                break;
              }
              if (is_selected)
              {
                ImGui::SetItemDefaultFocus();
              }
            }
          }
          ImGui::EndCombo();
        }
        style.Colors[ImGuiCol_FrameBg] = originalColor;
        style.Colors[ImGuiCol_FrameBgHovered] = originalHColor;
        style.Colors[ImGuiCol_Button] = originalBColor;
        style.Colors[ImGuiCol_ButtonHovered] = originalBHColor;
      }

      if (!toDeleteList.empty()) {
        for (const std::string& tds : toDeleteList)
        {
          auto it = std::find_if(allScripts->mScriptList.begin(), allScripts->mScriptList.end(), [tds](const Mono::ScriptInstance pair) { return pair.mScriptName == tds; });
          allScripts->mScriptList.erase(it);
        }
      }
    }

    WindowEnd(isOpen);
    return modified;
  }

  bool Inspector::TagComponentWindow(ECS::Entity entity, bool highlight) {
    bool const isOpen{ WindowBegin<Component::Tag>("Tag", highlight) };
    bool modified{ false };

    if (isOpen) {
      std::string tag{ entity.GetTag() };
      ImGui::PushFont(mStyler.GetCustomFont(GUI::MONTSERRAT_SEMIBOLD));

      bool isEntityActive = entity.IsActive();
      if (ImGui::Checkbox("##IsActiveCheckbox", &isEntityActive)) {
        entity.SetIsActive(isEntityActive);
      }
      ImGui::SameLine();
      ImGui::Text(" ");
      ImGui::SameLine();

      ImGui::SetNextItemWidth(INPUT_SIZE);
      if (ImGui::InputText("##TagComponentTag", &tag, ImGuiInputTextFlags_EnterReturnsTrue)) {
        entity.SetTag(tag);
        modified = true;
      }
      ImGui::PopFont();

      DrawAddButton();
    }

    WindowEnd(isOpen);
    return modified;
  }

  bool Inspector::TextComponentWindow(ECS::Entity entity, bool highlight) {
    bool const isOpen{ WindowBegin<Component::Text>("Text", highlight) };
    bool modified{ false };

    if (isOpen) {
      ImGui::Text("Usage: Must be child of an Entity with the \"Canvas\" Component");

      auto& text = entity.GetComponent<Component::Text>();
      float inputWidth{ CalcInputWidth(60.f) };

      ImGui::BeginTable("TextTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth);

      NextRowTable("");
      ImVec2 boxSize = ImVec2(200.0f, 40.0f); // Width and height of the box
      ImVec2 cursorPos = ImGui::GetCursorScreenPos();
      ImVec2 boxEnd = ImVec2(cursorPos.x + boxSize.x, cursorPos.y + boxSize.y);

      // Draw a child window to act as the box
      ImGui::BeginChild("DragDropTargetBox", boxSize, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

      // Get draw list and add a thin black border around the box
      ImGui::GetWindowDrawList()->AddRect(cursorPos, boxEnd, IM_COL32(0, 0, 0, 255), 0.0f, 0, 1.0f);

      // Center the text inside the box
      ImVec2 textSize = ImGui::CalcTextSize("Drag here to add font");
      ImVec2 textPos = ImVec2(
        cursorPos.x + (boxSize.x - textSize.x) * 0.5f,
        cursorPos.y + (boxSize.y - textSize.y) * 0.5f
      );
      ImGui::SetCursorScreenPos(textPos);
      ImGui::TextUnformatted("Drag here to add font");
      ImGui::EndChild();

      if (ImGui::BeginDragDropTarget()) {
        ImGuiPayload const* drop = ImGui::AcceptDragDropPayload(AssetPayload::sAssetDragDropPayload);
        if (drop) {
          AssetPayload assetPayload{ reinterpret_cast<const char*>(drop->Data) };
          if (assetPayload.mAssetType == AssetPayload::FONT) {
            text.textAsset = IGE_ASSETMGR.LoadRef<IGE::Assets::FontAsset>(assetPayload.GetFilePath());
            text.fontFamilyName = assetPayload.GetFileName();
            text.newLineIndicesUpdatedFlag = false;
            modified = true;
          }
        }
        ImGui::EndDragDropTarget();
      }

      std::string fontText = (text.fontFamilyName == "None") ? "[None]" : text.fontFamilyName;
      NextRowTable("Font Family");
      ImGui::BeginDisabled();
      ImGui::InputText("##FontTextInput", &fontText);
      ImGui::EndDisabled();
      
      NextRowTable("Color");
      if (ImGui::ColorEdit4("##TextColor", &text.color[0], ImGuiColorEditFlags_NoAlpha)) {
        modified = true;
      }

      NextRowTable("Text Input");
      if (ImGui::InputTextMultiline("##TextInput", &text.textContent)) {
        text.newLineIndicesUpdatedFlag = false;
        modified = true;
      }

      NextRowTable("Scale");
      if (ImGui::DragFloat("##TextScale", &text.scale, .001f, 0.f, 2.f)) {
        modified = true;
      }

      NextRowTable("Multi-Line Space Offset");
      if (ImGui::DragFloat("##MultiLineSpacingOffset", &text.multiLineSpacingOffset, .01f, -2.f, 2.f)) {
        modified = true;
      }

      NextRowTable("Text Alignment");
      if (ImGui::RadioButton("Left##TextAlignment", text.alignment == Component::Text::LEFT)) {
        text.alignment = Component::Text::LEFT;
        text.newLineIndicesUpdatedFlag = false;
        modified = true;
      }

      if (ImGui::RadioButton("Right##TextAlignment", text.alignment == Component::Text::RIGHT)) {
        text.alignment = Component::Text::RIGHT;
        text.newLineIndicesUpdatedFlag = false;
        modified = true;
      }

      if (ImGui::RadioButton("Center##TextAlignment", text.alignment == Component::Text::CENTER)) {
        text.alignment = Component::Text::CENTER;
        text.newLineIndicesUpdatedFlag = false;
        modified = true;
      }

      NextRowTable("Textbox Enabled");
      if (ImGui::Checkbox("##TextBoxEnabled", &text.textBoxEnabledFlag)) {
        text.newLineIndicesUpdatedFlag = false;
        modified = true;
      }

      ImGui::EndTable();

      if (text.textBoxEnabledFlag) {
        inputWidth = CalcInputWidth(50.f) / 3.f;
        ImGui::BeginTable("Text Box Dimensions", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);
        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
        ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, inputWidth);
        ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthFixed, inputWidth);
        ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthFixed, inputWidth);
        ImGui::TableHeadersRow();

        if (ImGuiHelpers::TableInputFloat3("Textbox Dimensions", &text.textBoxDimensions.x, inputWidth, false, -100.f, 100.f, 0.1f)) {
          text.newLineIndicesUpdatedFlag = false;
          modified = true;
        }
        ImGui::EndTable();
      }
    }

    WindowEnd(isOpen);
    return modified;
  }

  bool Inspector::TransformComponentWindow(ECS::Entity entity, bool highlight) {
    bool const isOpen{ WindowBegin<Component::Transform>("Transform", highlight) };
    bool modified{ false };

    if (isOpen) {
      Component::Transform& transform = entity.GetComponent<Component::Transform>();

      float const inputWidth{ CalcInputWidth(50.f) / 3.f };

      BeginVec3Table("LocalTransformTable", inputWidth);

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

      EndVec3Table();

      BeginVec3Table("WorldTransformTable", inputWidth);

      // only allow local transform to be modified
      glm::vec3 worldRot{ transform.GetWorldEulerAngles() };
      ImGui::BeginDisabled();
      ImGuiHelpers::TableInputFloat3("World Position", &transform.worldPos[0], inputWidth, false, -100.f, 100.f, 0.1f);
      ImGuiHelpers::TableInputFloat3("World Rotation", &worldRot[0], inputWidth, false, 0.f, 360.f, 0.1f);
      ImGuiHelpers::TableInputFloat3("World Scale", &transform.worldScale[0], inputWidth, false, 0.001f, 100.f, 1.f);
      ImGui::EndDisabled();

      EndVec3Table();
    }

    WindowEnd(isOpen);
    return modified;
  }

  bool Inspector::CanvasComponentWindow(ECS::Entity entity, bool highlight){
      bool const isOpen{ WindowBegin<Component::Canvas>("Canvas", highlight) };
      bool modified{ false };

      if (isOpen) {
          Component::Canvas& canvas = entity.GetComponent<Component::Canvas>();
          float const inputWidth{ CalcInputWidth(60.f) };
          // Start a table for organizing the color and textureAsset inputs
          ImGui::BeginTable("ImageTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

          ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
          ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth);

          NextRowTable("Toggle Visiblity");
          if (ImGui::Checkbox("##IsActive", &canvas.isActive)) {
              modified = true;
          }
          ImGui::EndTable();
      }

      WindowEnd(isOpen);
      return modified;
  }

  bool Inspector::ImageComponentWindow(ECS::Entity entity, bool highlight) {
      bool const isOpen{ WindowBegin<Component::Image>("Image", highlight) };
      bool modified{ false };

      if (isOpen) {
          Component::Image& image = entity.GetComponent<Component::Image>();

          float const inputWidth{ CalcInputWidth(60.f) };

          // Start a table for organizing the color and textureAsset inputs
          ImGui::BeginTable("ImageTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

          ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
          ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth);

          // Color input
          NextRowTable("Color");
          if (ImGui::ColorEdit4("##ImageColor", &image.color[0])) {
              modified = true;
          }

          // Texture Asset input - assuming textureAsset is a GUID or string
          


          NextRowTable("Texture Asset");
          static std::string textureText;
          try {
              textureText = (image.textureAsset) ? IGE_ASSETMGR.GUIDToPath(image.textureAsset) : "[None]: Drag in a Texture";
          }
          catch (Debug::ExceptionBase& e) {
              // If the GUID is not found, log the exception and set a default message
              textureText = "[Invalid GUID]: Unable to load texture";
              e.LogSource();
             ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), e.what());
          }

          
          //(image.textureAsset) ? IGE_ASSETMGR.GUIDToPath(image.textureAsset) : "[None]: Drag in a Texture";

          //catch (const Debug::ExceptionBase& e) {

          //}

          ImGui::BeginDisabled();
          ImGui::InputText("##TextureAsset", &textureText);
          ImGui::EndDisabled();

          if (ImGui::BeginDragDropTarget()) {
              ImGuiPayload const* drop = ImGui::AcceptDragDropPayload(AssetPayload::sAssetDragDropPayload);
              if (drop) {
                  AssetPayload assetPayload{ reinterpret_cast<const char*>(drop->Data) };
                  if (assetPayload.mAssetType == AssetPayload::SPRITE) {
                      image.textureAsset = IGE_ASSETMGR.LoadRef<IGE::Assets::TextureAsset>(assetPayload.GetFilePath());
                      textureText = assetPayload.GetFileName();  // Display the file name in the UI
                      modified = true;
                  }
              }
              ImGui::EndDragDropTarget();
          }

          ImGui::EndTable();
      }

      WindowEnd(isOpen);
      return modified;
  }

  bool Inspector::CameraComponentWindow(ECS::Entity entity, bool highlight)
  {
      bool const isOpen{ WindowBegin<Component::Camera>("Camera", highlight) };
      bool modified{ false };

      if (isOpen) {
          Component::Camera& camera = entity.GetComponent<Component::Camera>();

          float const inputWidth{ CalcInputWidth(60.f) };

          // Start a table for organizing the color and textureAsset inputs
          ImGui::BeginTable("CameraTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

          ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
          ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth);

          // Color input
          NextRowTable("Projection Type");
          //if (ImGui::ColorEdit4("##ProjType", &camera.projType)) {
          //    modified = true;
          //}

          NextRowTable("Yaw");
          if (ImGui::DragFloat("##Yaw", &camera.yaw, 1.f, -180.f, 180.f)) {
              modified = true;
          }
          NextRowTable("Pitch");
          if (ImGui::DragFloat("##Pitch", &camera.pitch, 1.f, -180.f, 180.f)) {
              modified = true;
          }
          NextRowTable("FOV");
          if (ImGui::DragFloat("##FOV", &camera.fov, 1.f, 0.f, 180.f)) {
              modified = true;
          }
          NextRowTable("Aspect Ratio");

          NextRowTable("Near Clip");
          if (ImGui::DragFloat("##Near", &camera.nearClip, 5.f, -100.f, 0.f)) {
              modified = true;
          }
          NextRowTable("Far Clip");
          if (ImGui::DragFloat("##Far", &camera.farClip, 5.f, 0.f, 1000.f)) {
              modified = true;
          }
          


          ImGui::EndTable();
      }

      WindowEnd(isOpen);
      return modified;
  }

  bool Inspector::MeshComponentWindow(ECS::Entity entity, bool highlight) {
    bool const isOpen{ WindowBegin<Component::Mesh>("Mesh", highlight) };
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
                mesh.meshSource = IGE_ASSETMGR.LoadRef<IGE::Assets::ModelAsset>(selected);
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
            mesh.meshSource = IGE_ASSETMGR.LoadRef<IGE::Assets::ModelAsset>(assetPayload.GetFilePath());
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

  bool Inspector::RigidBodyComponentWindow(ECS::Entity entity, bool highlight) {
    bool const isOpen{ WindowBegin<Component::RigidBody>("RigidBody", highlight) };
    bool modified{ false };

    if (isOpen) {

      // Assuming 'rigidBody' is an instance of RigidBody
      Component::RigidBody& rigidBody{ entity.GetComponent<Component::RigidBody>() };

      float const inputWidth{ CalcInputWidth(50.f) / 3.f };

      BeginVec3Table("RigidBodyTable", inputWidth);

      if (ImGuiHelpers::TableInputFloat3("Velocity", &rigidBody.velocity.x, inputWidth, false, -100.f, 100.f, 0.1f)) {
        IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::VELOCITY);
        modified = true;
      }
      if (ImGuiHelpers::TableInputFloat3("Angular Velocity", &rigidBody.angularVelocity.x, inputWidth, false, -100.f, 100.f, 0.1f)) {
        IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::ANGULAR_VELOCITY);
        modified = true;
      }

      EndVec3Table();

      ImGui::BeginTable("ShapeSelectionTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth * 3);

      NextRowTable("Static Friction");
      if (ImGui::DragFloat("##RigidBodyStaticFriction", &rigidBody.staticFriction, 0.01f, 0.0f, 1.0f)) {
        IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::STATIC_FRICTION);
        modified = true;
      }

      NextRowTable("Dynamic Friction");
      if (ImGui::DragFloat("##RigidBodyDynamicFriction", &rigidBody.dynamicFriction, 0.01f, 0.0f, 1.0f)) {
          IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::STATIC_FRICTION);
          modified = true;
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
          modified = true;
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

  bool Inspector::AudioListenerComponentWindow(ECS::Entity entity, bool highlight)
  {
      bool const isOpen{ WindowBegin<Component::AudioListener>("AudioListener", highlight) };
      bool modified{ false };

      if (isOpen) {
          ImGui::Text("Virtual \"Ear\" for the world");
      }

      WindowEnd(isOpen);
      return modified;
  }

  bool Inspector::AudioSourceComponentWindow(ECS::Entity entity, bool highlight) {
      bool const isOpen{ WindowBegin<Component::AudioSource>("AudioSource", highlight) };
      bool modified{ false };

      if (isOpen) {
          Component::AudioSource& audioSource{ entity.GetComponent<Component::AudioSource>() };
          float const inputWidth{ CalcInputWidth(50.f) / 3.f };
          ImVec2 const boxSize = ImVec2(200.0f, 40.0f); // Width and height of the box
          ImVec2 const cursorPos = ImGui::GetCursorScreenPos();
          ImVec2 const boxEnd = ImVec2(cursorPos.x + boxSize.x, cursorPos.y + boxSize.y);

          // Draw a child window to act as the box
          ImGui::BeginChild("DragDropTargetBox", boxSize, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

          // Get draw list and add a thin black border around the box
          ImGui::GetWindowDrawList()->AddRect(cursorPos, boxEnd, IM_COL32(0, 0, 0, 255), 0.0f, 0, 1.0f);

          // Center the text inside the box
          ImVec2 const textSize = ImGui::CalcTextSize("Drag here to add sound");
          ImVec2 const textPos = ImVec2(
              cursorPos.x + (boxSize.x - textSize.x) * 0.5f,
              cursorPos.y + (boxSize.y - textSize.y) * 0.5f
          );
          ImGui::SetCursorScreenPos(textPos);
          ImGui::TextUnformatted("Drag here to add sound");
          ImGui::EndChild();
          if (ImGui::BeginDragDropTarget())
          {
              ImGuiPayload const* drop = ImGui::AcceptDragDropPayload(AssetPayload::sAssetDragDropPayload);
              if (drop) {
                  AssetPayload assetPayload{ reinterpret_cast<const char*>(drop->Data) };
                  if (assetPayload.mAssetType == AssetPayload::AUDIO) {
                      //auto meshSrc{ std::make_shared<Graphics::Mesh>(Graphics::MeshFactory::CreateModelFromImport(assetPayload.GetFilePath())) };
                      auto fp{ assetPayload.GetFilePath() };
                      audioSource.CreateSound(fp);
                      modified = true;
                  }
              }
              ImGui::EndDragDropTarget();
          }
          // Iterate through each AudioInstance in the sounds map
          static std::string editingKey = ""; // Track the current sound name being edited
          static char renameBuffer[128] = "";
          for (auto& [currentName, audioInstance] : audioSource.sounds) {
              // Display Sound Name
              ImGui::Text("%s", currentName.c_str());

              // Begin a Tree Node for sound properties
              if (ImGui::TreeNode("Sound Properties")) {
                  // Table for 3D position
                  if (BeginVec3Table("PositionTable", inputWidth)) {
                      if (ImGuiHelpers::TableInputFloat3("Position", &audioInstance.playSettings.position.x, inputWidth, false, -100.f, 100.f, 0.1f)) {
                          modified = true;
                      }
                      EndVec3Table();
                  }

                  // Table for single properties
                  if (ImGui::BeginTable("SoundPropertyTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit)) {
                      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
                      ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, inputWidth * 3);

                      NextRowTable("Volume");
                      if (ImGui::DragFloat("##Volume", &audioInstance.playSettings.volume, 0.01f, 0.0f, 1.0f)) {
                          modified = true;
                      }

                      NextRowTable("Pitch");
                      if (ImGui::DragFloat("##Pitch", &audioInstance.playSettings.pitch, 0.01f, 0.1f, 3.0f)) {
                          modified = true;
                      }

                      NextRowTable("Pan");
                      if (ImGui::DragFloat("##Pan", &audioInstance.playSettings.pan, 0.01f, -1.0f, 1.0f)) {
                          modified = true;
                      }

                      NextRowTable("Doppler Level");
                      if (ImGui::DragFloat("##DopplerLevel", &audioInstance.playSettings.dopplerLevel, 0.01f, 0.0f, 5.0f)) {
                          modified = true;
                      }

                      NextRowTable("Min Distance");
                      if (ImGui::DragFloat("##MinDistance", &audioInstance.playSettings.minDistance, 0.1f, 0.0f, 1000.0f)) {
                          modified = true;
                      }

                      NextRowTable("Max Distance");
                      if (ImGui::DragFloat("##MaxDistance", &audioInstance.playSettings.maxDistance, 0.1f, 0.0f, 1000.0f)) {
                          modified = true;
                      }
                      // Checkboxes for Mute, Loop, and Play on Awake
                      NextRowTable("Mute");
                      if (ImGui::Checkbox("##Mute", &audioInstance.playSettings.mute)) {
                          modified = true;
                      }

                      NextRowTable("Loop");
                      if (ImGui::Checkbox("##Loop", &audioInstance.playSettings.loop)) {
                          modified = true;
                      }

                      NextRowTable("Play On Awake");
                      if (ImGui::Checkbox("##PlayOnAwake", &audioInstance.playSettings.playOnAwake)) {
                          modified = true;
                      }

                      ImGui::EndTable();
                  }

                  // Table for Rolloff Type
                  static const char* rolloffTypes[] = { "Linear", "Logarithmic" };
                  int currentRolloff = static_cast<int>(audioInstance.playSettings.rolloffType);

                  ImGui::BeginTable("RolloffTypeTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);
                  ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
                  ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, inputWidth * 3);

                  NextRowTable("Rolloff Type");
                  if (ImGui::Combo("##RolloffType", &currentRolloff, rolloffTypes, IM_ARRAYSIZE(rolloffTypes))) {
                      audioInstance.playSettings.rolloffType = static_cast<IGE::Audio::SoundInvokeSetting::RolloffType>(currentRolloff);
                      modified = true;
                  }

                  ImGui::EndTable();

                  ImGui::TreePop();
              }
          }
      }

      WindowEnd(isOpen);
      return modified;
  }

  bool Inspector::BoxColliderComponentWindow(ECS::Entity entity, bool highlight) {
    bool const isOpen{ WindowBegin<Component::BoxCollider>("Box Collider", highlight) };
    bool modified{ false };

    if (isOpen) {
      Component::BoxCollider& collider{ entity.GetComponent<Component::BoxCollider>() };
      float const inputWidth{ CalcInputWidth(50.f) / 3.f };

      BeginVec3Table("BoxColliderTable", inputWidth);

      // Modify the scale (vec3 input)
      if (ImGuiHelpers::TableInputFloat3("Box Scale", &collider.scale.x, inputWidth, false, 0.1f, 10.0f, 0.1f)) {
          IGE::Physics::PhysicsSystem::GetInstance()->ChangeBoxColliderVar(entity);
          modified = true;
      }

      // Modify positionOffset (vec3 input)
      if (ImGuiHelpers::TableInputFloat3("Box Position Offset", &collider.positionOffset.x, inputWidth, false, -100.f, 100.f, 0.1f)) {
          IGE::Physics::PhysicsSystem::GetInstance()->ChangeBoxColliderVar(entity);
          modified = true;
      }

      // Modify rotationOffset (vec3 input)
      if (ImGuiHelpers::TableInputFloat3("Box Rotation Offset", &collider.rotationOffset.x, inputWidth, false, -360.f, 360.f, 1.f)) {
          IGE::Physics::PhysicsSystem::GetInstance()->ChangeBoxColliderVar(entity);
          modified = true;
      }

      // End the table
      EndVec3Table();

      // Modify sensor (bool input)
      /*
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0); ImGui::Text("Sensor");
      ImGui::TableSetColumnIndex(1);  // Align in the X column
      if (ImGui::Checkbox("##Sensor", &collider.sensor)) {
          IGE::Physics::PhysicsSystem::GetInstance()->ChangeBoxColliderVar(entity);
          SetIsComponentEdited(true);
      }
      */

      ImGui::BeginTable("BoxSensorTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      NextRowTable("Box Sensor");
      if (ImGui::Checkbox("##BoxSensor", &collider.sensor)) {
        IGE::Physics::PhysicsSystem::GetInstance()->ChangeBoxColliderVar(entity);
        modified = true;
      }
      ImGui::EndTable();
    }

    WindowEnd(isOpen);
    return modified;
  }

  bool Inspector::SphereColliderComponentWindow(ECS::Entity entity, bool highlight)
  {
      bool const isOpen{ WindowBegin<Component::SphereCollider>("Sphere Collider", highlight) };
      bool modified{ false };

      if (isOpen) {
          // Assuming 'collider' is an instance of Collider
          Component::SphereCollider& collider{ entity.GetComponent<Component::SphereCollider>() };
          float contentSize = ImGui::GetContentRegionAvail().x;
          float charSize = ImGui::CalcTextSize("012345678901234").x;
          float inputWidth = (contentSize - charSize - 50) / 3;

          // Start a table for the Collider component
          BeginVec3Table("SphereColliderTable", inputWidth);

          // Modify the radius (single float input)
          ImGui::TableNextRow();
          ImGui::TableSetColumnIndex(0); ImGui::Text("Radius");
          ImGui::TableSetColumnIndex(1);  // Span across X column
          if (ImGui::DragFloat("##Radius", &collider.radius, 0.1f, 0.f, 100.f)) {
              IGE::Physics::PhysicsSystem::GetInstance()->ChangeSphereColliderVar(entity);
              modified = true;
          }

          // Modify positionOffset (vec3 input)
          if (ImGuiHelpers::TableInputFloat3("Sphere Position Offset", &collider.positionOffset.x, inputWidth, false, -100.f, 100.f, 0.1f)) {
              IGE::Physics::PhysicsSystem::GetInstance()->ChangeSphereColliderVar(entity);
              modified = true;
          }

          // Modify rotationOffset (vec3 input)
          if (ImGuiHelpers::TableInputFloat3("Sphere Rotation Offset", &collider.rotationOffset.x, inputWidth, false, -360.f, 360.f, 1.f)) {
              IGE::Physics::PhysicsSystem::GetInstance()->ChangeSphereColliderVar(entity);
              modified = true;
          }

          // End the table
          EndVec3Table();

          // Modify sensor (bool input)
          /*
          ImGui::TableNextRow();
          ImGui::TableSetColumnIndex(0); ImGui::Text("Sensor");
          ImGui::TableSetColumnIndex(1);  // Align in the X column
          if (ImGui::Checkbox("##Sensor", &collider.sensor)) {
              IGE::Physics::PhysicsSystem::GetInstance()->ChangeSphereColliderVar(entity);
              SetIsComponentEdited(true);
          }
          */

          ImGui::BeginTable("SphereSensorTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);
          ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
          ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth);
          NextRowTable("Sphere Sensor");
          if (ImGui::Checkbox("##SphereSensor", &collider.sensor)) {
            IGE::Physics::PhysicsSystem::GetInstance()->ChangeSphereColliderVar(entity);
            modified = true;
          }
          ImGui::EndTable();
      }

      WindowEnd(isOpen);
      return modified;
  }

  bool Inspector::CapsuleColliderComponentWindow(ECS::Entity entity, bool highlight)
  {
      bool const isOpen{ WindowBegin<Component::CapsuleCollider>("Capsule Collider", highlight) };
      bool modified{ false };

      if (isOpen) {
          // Assuming 'collider' is an instance of Collider
          Component::CapsuleCollider& collider{ entity.GetComponent<Component::CapsuleCollider>() };
          float contentSize = ImGui::GetContentRegionAvail().x;
          float charSize = ImGui::CalcTextSize("012345678901234").x;
          float inputWidth = (contentSize - charSize - 50) / 3;

          // Start a table for the Collider component
          BeginVec3Table("CapsuleColliderTable", inputWidth);

          // Modify the radius (single float input)
          ImGui::TableNextRow();
          ImGui::TableSetColumnIndex(0); ImGui::Text("Capsule Radius");
          ImGui::TableSetColumnIndex(1);  // Span across X column
          if (ImGui::DragFloat("##Radius", &collider.radius, 0.1f, 0.1f, 100.f)) {
              IGE::Physics::PhysicsSystem::GetInstance()->ChangeCapsuleColliderVar(entity);
              modified = true;
          }

          // Modify the halfHeight (single float input)
          ImGui::TableNextRow();
          ImGui::TableSetColumnIndex(0); ImGui::Text("Capsule Half Height");
          ImGui::TableSetColumnIndex(1);  // Span across X column
          if (ImGui::DragFloat("##HalfHeight", &collider.halfheight, 0.1f, 0.1f, 100.f)) {
              IGE::Physics::PhysicsSystem::GetInstance()->ChangeCapsuleColliderVar(entity);
              modified = true;
          }

          // Modify positionOffset (vec3 input)
          if (ImGuiHelpers::TableInputFloat3("Capsule Position Offset", &collider.positionOffset.x, inputWidth, false, -100.f, 100.f, 0.1f)) {
              IGE::Physics::PhysicsSystem::GetInstance()->ChangeCapsuleColliderVar(entity);
              modified = true;
          }

          // Modify rotationOffset (vec3 input)
          if (ImGuiHelpers::TableInputFloat3("Capsule Rotation Offset", &collider.rotationOffset.x, inputWidth, false, -360.f, 360.f, 1.f)) {
              IGE::Physics::PhysicsSystem::GetInstance()->ChangeCapsuleColliderVar(entity);
              modified = true;
          }

          // End the table
          EndVec3Table();

          // Modify sensor (bool input)
          //ImGui::TableNextRow();
          //ImGui::TableSetColumnIndex(0); 
          //ImGui::TableSetColumnIndex(1);  // Align in the X column

          ImGui::BeginTable("ColliderSensorTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);
          ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
          ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth);
          NextRowTable("Capsule Sensor");
          if (ImGui::Checkbox("##ColliderSensor", &collider.sensor)) {
              IGE::Physics::PhysicsSystem::GetInstance()->ChangeCapsuleColliderVar(entity);
              modified = true;
          }
          ImGui::EndTable();
      }

      WindowEnd(isOpen);
      return modified;
  }

  bool Inspector::LightComponentWindow(ECS::Entity entity, bool highlight) {
    bool const isOpen{ WindowBegin<Component::Light>("Light", highlight) };
    bool modified{ false };

    if (isOpen) {
      const std::vector<std::string> Lights{ "Directional","Spotlight" };
      //  // Assuming 'collider' is an instance of Collider
      Component::Light& light{ entity.GetComponent<Component::Light>() };
      float const inputWidth{ CalcInputWidth(50.f) }, vec3InputWidth{ inputWidth / 3.f };

      ImGui::BeginTable("##LightTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("Col1", ImGuiTableColumnFlags_WidthFixed, inputWidth);

      NextRowTable("Type");
      if (ImGui::BeginCombo("", Lights[light.type].c_str()))
      {
        for (int s{ 0 }; s < static_cast<int>(Component::LIGHT_COUNT); ++s)
        {
          if (Lights[s] != Lights[light.type])
          {
            bool is_selected = (Lights[light.type] == Lights[s]);
            if (ImGui::Selectable(Lights[s].c_str(), is_selected))
            {
              if (Lights[s] != Lights[light.type]) {
                light.type = static_cast<Component::LightType>(s);
              }
              modified = true;
              break;
            }
            if (is_selected)
            {
              ImGui::SetItemDefaultFocus();
            }
          }
        }
        ImGui::EndCombo();
      }

      NextRowTable("Color");
      if (ImGui::ColorEdit4("##LightCol", &light.color[0], ImGuiColorEditFlags_NoAlpha)) {
        modified = true;
      }

      NextRowTable("Intensity");
      if (ImGui::DragFloat("##In", &light.mLightIntensity, 0.5f, 0.f, FLT_MAX, "%.2f")) {
        modified = true;
      }
      if (light.type == Component::SPOTLIGHT)
      {

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Inner Angle");
        ImGui::TableNextColumn();
        if (ImGui::SliderFloat("##T", &light.mInnerSpotAngle, -360.f, 360.f, "%.f")) {
          modified = true;
        }


        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Output Angle");
        ImGui::TableNextColumn();
        if (ImGui::SliderFloat("##O", &light.mOuterSpotAngle, -360.f, 360.f, "%.f")) {
          modified = true;
        }


        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Range");
        ImGui::TableNextColumn();
        if (ImGui::DragFloat("##R", &light.mRange, 0.5f, 0.f, FLT_MAX)) {
          modified = true;
        }
      }
      // @TODO: Remove else block when shadow is added for spotlight
      else {
        NextRowTable("Cast Shadows");
        if (ImGui::Checkbox("##CastShadows", &light.castShadows)) {
          modified = true;
        }
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
          ImGui::SetTooltip("Note: Only 1 shadow-casting light is supported");
        }
      }
      ImGui::EndTable();

      if (light.castShadows && light.type == Component::LightType::DIRECTIONAL) {
        ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
        ImGui::PushFont(mStyler.GetCustomFont(GUI::MONTSERRAT_REGULAR));
        if (ImGui::TreeNodeEx("Shadows", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth)) {
          ImGui::PopFont();

          if (ImGui::BeginTable("##LightShadows2", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit)) {
            ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
            ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth);

            NextRowTable("Softness");
            if (ImGui::SliderInt("##SoftnessSlider", &light.softness, 0, 5)) {
              modified = true;
            }

            NextRowTable("Bias");
            if (ImGui::SliderFloat("##BiasSlider", &light.bias, 0.f, 2.f, "% .3f")) {
              modified = true;
            }

            NextRowTable("Near Plane");
            if (ImGui::IsItemHovered()) {
              ImGui::SetTooltip("How close the light is to the object (how much of the scene the light sees)");
            }
            if (ImGui::SliderFloat("##NearPlane", &light.nearPlaneMultiplier, 0.1f, 2.f, "% .2f")) {
              modified = true;
            }

            ImGui::EndTable();
          }

          ImGui::TreePop();
        }
        else {
          ImGui::PopFont();
        }
        ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
      }
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
        ImGui::TableSetupColumn("ComponentNames", ImGuiTableColumnFlags_WidthFixed, 220.f);
        
        // @TODO: EDIT WHEN NEW COMPONENTS
        DrawAddComponentButton<Component::AudioListener>("Audio Listener");
        DrawAddComponentButton<Component::AudioSource>("Audio Source");
        DrawAddComponentButton<Component::BoxCollider>("Box Collider");
        DrawAddComponentButton<Component::CapsuleCollider>("Capsule Collider");
        DrawAddComponentButton<Component::Layer>("Layer");
        DrawAddComponentButton<Component::MaterialComponent>("Material");
        DrawAddComponentButton<Component::Mesh>("Mesh");
        DrawAddComponentButton<Component::RigidBody>("RigidBody");
        DrawAddComponentButton<Component::Script>("Script");
        DrawAddComponentButton<Component::SphereCollider>("Sphere Collider");
        DrawAddComponentButton<Component::Tag>("Tag");
        DrawAddComponentButton<Component::Text>("Text");
        DrawAddComponentButton<Component::Transform>("Transform");
        DrawAddComponentButton<Component::Light>("Light");
        DrawAddComponentButton<Component::Canvas>("Canvas");
        DrawAddComponentButton<Component::Image>("Image");
        DrawAddComponentButton<Component::Camera>("Camera");

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

  bool Inspector::BeginVec3Table(const char* fieldName, float inputWidth) {
    if (!ImGui::BeginTable(fieldName, 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit)) {
      return false;
    }

    ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
    ImGui::TableSetupColumn(" X", ImGuiTableColumnFlags_WidthFixed, inputWidth);
    ImGui::TableSetupColumn(" Y", ImGuiTableColumnFlags_WidthFixed, inputWidth);
    ImGui::TableSetupColumn(" Z", ImGuiTableColumnFlags_WidthFixed, inputWidth);
    ImGui::TableHeadersRow();

    return true;
  }
  void Inspector::EndVec3Table() { ImGui::EndTable(); }

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
    //propertyName = "##" + propertyName;
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
