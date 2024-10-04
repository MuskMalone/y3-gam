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

#include <Physics/PhysicsSystem.h>
#include <functional>
#include <Reflection/ComponentTypes.h>
#include <Events/EventManager.h>
#include <Graphics/MeshFactory.h>
#include <Graphics/Mesh.h>

namespace GUI {
  Inspector::Inspector(std::string const& name) : GUIWindow(name),
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
          trans.modified = true;
          SetIsComponentEdited(true);

          if (prefabOverride) {
            if (prefabOverride->subDataId == Prefabs::PrefabSubData::BasePrefabId) {
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

      if (currentEntity.HasComponent<Component::Collider>()) {
        rttr::type const colliderType{ rttr::type::get<Component::Collider>() };
        componentOverriden = prefabOverride && prefabOverride->IsComponentModified(colliderType);

        if (ColliderComponentWindow(currentEntity, std::string(ICON_FA_BOMB), componentOverriden)) {
          SetIsComponentEdited(true);
          if (prefabOverride) {
            prefabOverride->AddComponentModification(currentEntity.GetComponent<Component::Collider>());
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
      auto& layer = entity.GetComponent<Component::Layer>();

      // @TODO: TEMP, TO BE REPLACED WITH ACTUAL LAYERS
      std::vector<const char*> availableLayers{ "Default", "Test1", "Test2" };

      float contentSize = ImGui::GetContentRegionAvail().x;
      float charSize = ImGui::CalcTextSize("012345678901234").x;
      float inputWidth = (contentSize - charSize - 60);

      ImGui::BeginTable("LayerTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Assigned Layer");

      ImGui::TableSetColumnIndex(1);
      ImGui::SetNextItemWidth(INPUT_SIZE);

      if (ImGui::BeginCombo("##LayerName", layer.name.c_str())) {
        for (const char* layerName : availableLayers) {
          if (ImGui::Selectable(layerName)) {
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
      auto& material = entity.GetComponent<Component::Material>();

      float contentSize = ImGui::GetContentRegionAvail().x;
      float charSize = ImGui::CalcTextSize("012345678901234").x;
      float inputWidth = (contentSize - charSize - 60);

      ImGui::BeginTable("MaterialTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Color");
      ImGui::TableSetColumnIndex(1);
      ImGui::SetNextItemWidth(INPUT_SIZE);

      glm::vec3 color = material.material->GetAlbedoColor();

      if (ImGui::ColorEdit3("##AlebdoColor", &color[0])) {
        material.material->SetAlbedoColor(color);
        modified = true;
      }

      ImGui::EndTable();
    }

    WindowEnd(isOpen);
    return false;
  }

  bool Inspector::ScriptComponentWindow(ECS::Entity entity, std::string const& icon, bool highlight) {
    bool const isOpen{ WindowBegin<Component::Script>("Script", icon, highlight) };
    bool modified{ false };

    if (isOpen) {
      auto& script = entity.GetComponent<Component::Script>();

      // @TODO: TEMP, TO BE REPLACED WITH ACTUAL SCRIPTS
      std::vector<const char*> availableScripts{ "None", "Test1", "Test2" };

      float contentSize = ImGui::GetContentRegionAvail().x;
      float charSize = ImGui::CalcTextSize("012345678901234").x;
      float inputWidth = (contentSize - charSize - 60);

      ImGui::BeginTable("ScriptTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Assigned Script");

      ImGui::TableSetColumnIndex(1);
      ImGui::SetNextItemWidth(INPUT_SIZE);

      if (ImGui::BeginCombo("##ScriptName", script.name.c_str())) {
        for (const char* scriptName : availableScripts) {
          if (ImGui::Selectable(scriptName)) {
            script.name = scriptName;
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

      float contentSize = ImGui::GetContentRegionAvail().x;
      float charSize = ImGui::CalcTextSize("012345678901234").x;
      float inputWidth = (contentSize - charSize - 60);

      ImGui::BeginTable("TextTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Font Family");
      ImGui::TableSetColumnIndex(1);
      ImGui::SetNextItemWidth(INPUT_SIZE);
      if (ImGui::BeginCombo("##TextName", text.fontName.c_str())) {
        for (const char* fontName : availableFonts) {
          if (ImGui::Selectable(fontName)) {
            text.fontName = fontName;
            modified = true;
          }
        }
        ImGui::EndCombo();
      }
      
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Color");
      ImGui::TableSetColumnIndex(1);
      ImGui::SetNextItemWidth(INPUT_SIZE);
      if (ImGui::ColorEdit4("##TextColor", &text.color[0])) {
        modified = true;
      }

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Text Input");
      ImGui::TableSetColumnIndex(1);
      ImGui::SetNextItemWidth(INPUT_SIZE);
      if (ImGui::InputTextMultiline("##TextInput", &text.textContent)) {
        modified = true;
      }

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Scale");
      ImGui::TableSetColumnIndex(1);
      ImGui::SetNextItemWidth(INPUT_SIZE);
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

      float contentSize = ImGui::GetContentRegionAvail().x;
      float charSize = ImGui::CalcTextSize("012345678901234").x;
      float inputWidth = (contentSize - charSize - 50) / 3;

      ImGui::BeginTable("LocalTransformTable", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableHeadersRow();

      // @TODO: Replace min and max with the world min and max
      if (ImGuiHelpers::TableInputFloat3("Translation", &transform.position[0], inputWidth, false, -100.f, 100.f, 0.1f)) {
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
      ImGuiHelpers::TableInputFloat3("World Translation", &transform.worldPos[0], inputWidth, false, -100.f, 100.f, 0.1f);
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
      static const std::vector<const char*> meshNames{
        "None", "Cube", "Plane"
      };
      Component::Mesh& mesh{ entity.GetComponent<Component::Mesh>() };

      if (ImGui::BeginCombo("##MeshSelection", mesh.meshName.c_str())) {
        for (unsigned i{}; i < meshNames.size(); ++i) {
          const char* selected{ meshNames[i] };
          if (ImGui::Selectable(selected)) {
            if (i != 0) {
              mesh.mesh = std::make_shared<Graphics::Mesh>(Graphics::MeshFactory::CreateModelFromString(selected));
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
      // @TODO: ABSTRACT MORE; MAKE IT EASIER TO ADD A MESH
      else if (ImGui::BeginDragDropTarget())
      {
        ImGuiPayload const* drop = ImGui::AcceptDragDropPayload(AssetPayload::sAssetDragDropPayload);
        if (drop) {
          AssetPayload assetPayload{ reinterpret_cast<const char*>(drop->Data) };
          if (assetPayload.mAssetType == AssetPayload::MODEL) {
            auto meshSrc{ std::make_shared<Graphics::Mesh>(Graphics::MeshFactory::CreateModelFromImport(assetPayload.GetFilePath())) };
            mesh.mesh = std::move(meshSrc);
            mesh.meshName = assetPayload.GetFileName();
          }
        }
        ImGui::EndDragDropTarget();
      }

    }

    WindowEnd(isOpen);
    return modified;
  }

  void Inspector::WindowEnd(bool const isOpen) {
    if (isOpen) {
      ImGui::TreePop();
      ImGui::PopFont();
    }

    ImGui::Separator();
  }

  bool Inspector::RigidBodyComponentWindow(ECS::Entity entity, std::string const& icon, bool highlight) {
    bool const isOpen{ WindowBegin<Component::RigidBody>("RigidBody", icon, highlight) };
    bool modified{ false };

    if (isOpen) {
      // Assuming 'rigidBody' is an instance of RigidBody
      Component::RigidBody& rigidBody{ entity.GetComponent<Component::RigidBody>() };

      float contentSize = ImGui::GetContentRegionAvail().x;
      float charSize = ImGui::CalcTextSize("012345678901234").x;
      float inputWidth = (contentSize - charSize - 50) / 3;

      ImGui::BeginTable("RigidBodyTable", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableHeadersRow();

      if (ImGuiHelpers::TableInputFloat3("Velocity", rigidBody.velocity.mF32, inputWidth, false, -100.f, 100.f, 0.1f)) {
        IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::VELOCITY);
        modified = true;
      }
      if (ImGuiHelpers::TableInputFloat3("Angular Velocity", rigidBody.angularVelocity.mF32, inputWidth, false, -100.f, 100.f, 0.1f)) {
        IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::ANGULAR_VELOCITY);
        modified = true;
      }

      ImGui::EndTable();

      ImGui::BeginTable("ShapeSelectionTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth * 3);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Friction");
      ImGui::TableSetColumnIndex(1);
      ImGui::SetNextItemWidth(INPUT_SIZE);
      if (ImGui::DragFloat("##RigidBodyFriction", &rigidBody.friction, 0.01f, 0.0f, 1.0f)) {
        IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::FRICTION);
        modified = true;
      }

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Restitution");
      ImGui::TableSetColumnIndex(1);
      ImGui::SetNextItemWidth(INPUT_SIZE);
      if (ImGui::DragFloat("##RigidBodyRestitution", &rigidBody.restitution, 0.01f, 0.0f, 1.0f)) {
        IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::RESTITUTION);
        modified = true;
      }

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Gravity Factor");
      ImGui::TableSetColumnIndex(1);
      ImGui::SetNextItemWidth(INPUT_SIZE);
      if (ImGui::DragFloat("##RigidBodyGravityFactor", &rigidBody.gravityFactor, 0.01f, 0.0f, 10.0f)) {
        IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::GRAVITY_FACTOR);
        modified = true;
      }

      // Motion Type Selection
      static const char* motionTypes[] = { "Static", "Kinematic", "Dynamic" };
      int currentMotionType = static_cast<int>(rigidBody.motionType);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Motion Type");
      ImGui::TableSetColumnIndex(1);
      ImGui::SetNextItemWidth(INPUT_SIZE);
      if (ImGui::Combo("##Motion Type", &currentMotionType, motionTypes, IM_ARRAYSIZE(motionTypes))) {
        rigidBody.motionType = static_cast<JPH::EMotionType>(currentMotionType);
        IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::MOTION);
        modified = true;
      }

      ImGui::EndTable();
    }

    WindowEnd(isOpen);
    return modified;
  }

  bool Inspector::ColliderComponentWindow(ECS::Entity entity, std::string const& icon, bool highlight) {
    bool const isOpen{ WindowBegin<Component::Collider>("Collider", icon, highlight) };
    bool modified{ false };

    if (isOpen) {
      // Assuming 'collider' is an instance of Collider
      Component::Collider& collider{ entity.GetComponent<Component::Collider>() };
      float contentSize = ImGui::GetContentRegionAvail().x;
      float charSize = ImGui::CalcTextSize("012345678901234").x;
      float inputWidth = (contentSize - charSize - 50) / 3;

      ImGui::BeginTable("ColliderTable", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableHeadersRow();

      if (ImGuiHelpers::TableInputFloat3("Scale", collider.scale.mF32, inputWidth, false, 0.f, 100.f, 1.f)) {
        modified = true;
      }
      if (ImGuiHelpers::TableInputFloat3("Position Offset", collider.positionOffset.mF32, inputWidth, false, -100.f, 100.f, 0.1f)) {
        modified = true;
      }
      if (ImGuiHelpers::TableInputFloat3("Rotation Offset", collider.rotationOffset.mF32, inputWidth, false, 0.f, 360.f, 0.1f)) {
        modified = true;
      }

      ImGui::EndTable();

      // Shape Type Selection
      static const char* shapeTypes[] = { "Unknown", "Sphere", "Capsule", "Box", "Triangle", "ConvexHull", "Mesh", "HeightField", "Compound" };
      int currentShapeType = static_cast<int>(collider.type);

      ImGui::BeginTable("ShapeSelectionTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, inputWidth * 3);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Shape Type");
      ImGui::TableSetColumnIndex(1);
      ImGui::SetNextItemWidth(INPUT_SIZE);
      if (ImGui::Combo("##ShapeType", &currentShapeType, shapeTypes, IM_ARRAYSIZE(shapeTypes))) {
        collider.type = static_cast<JPH::EShapeSubType>(currentShapeType);
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
        DrawAddComponentButton<Component::Collider>("Collider", ICON_FA_BOMB);
        DrawAddComponentButton<Component::Layer>("Layer", ICON_FA_LAYER_GROUP);
        DrawAddComponentButton<Component::Material>("Material", ICON_FA_GEM);
        DrawAddComponentButton<Component::Mesh>("Mesh", ICON_FA_CUBE);
        DrawAddComponentButton<Component::RigidBody>("RigidBody", ICON_FA_CAR);
        DrawAddComponentButton<Component::Script>("Script", ICON_FA_FILE_CODE);
        DrawAddComponentButton<Component::Tag>("Tag", ICON_FA_TAG);
        DrawAddComponentButton<Component::Text>("Text", ICON_FA_FONT);
        DrawAddComponentButton<Component::Transform>("Transform", std::string(ICON_FA_ROTATE));

        ImGui::EndTable();
      }

      ImGui::EndPopup();
    }
  }
} // namespace GUI