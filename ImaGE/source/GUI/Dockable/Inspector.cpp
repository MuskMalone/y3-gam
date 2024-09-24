#include <pch.h>
#ifndef IMGUI_DISABLE
#include "Inspector.h"

#include <imgui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include "Color.h"
#include "GUI/Helpers/ImGuiHelpers.h"

#include <Physics/PhysicsSystem.h>
#include <functional>
#include <Reflection/ComponentTypes.h>
#include <Events/EventManager.h>

namespace GUI {
  Inspector::Inspector(std::string const& name) : GUIWindow(name),
    mComponentOpenStatusMap{}, mStyler{ GUIManager::GetStyler() }, mObjFactory{Reflection::ObjectFactory::GetInstance()},
    mPreviousEntity{}, mIsComponentEdited{ false }, mFirstEdit{ false }, mEntityChanged{ false } {
    for (auto const& component : Reflection::gComponentTypes) {
      mComponentOpenStatusMap[component.get_name().to_string().c_str()] = true;
    }

    // get notified when scene is saved
    SUBSCRIBE_CLASS_FUNC(Events::EventType::SAVE_SCENE, &Inspector::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::SCENE_STATE_CHANGE, &Inspector::HandleEvent, this);
  }

  void Inspector::Run() {
    ImGui::Begin(mWindowName.c_str());
    ImGui::PushFont(mStyler.GetCustomFont(GUI::MONTSERRAT_SEMIBOLD));
    ECS::Entity const& currentEntity{ GUIManager::GetSelectedEntity() };
    
    if (currentEntity) {

      if (currentEntity != mPreviousEntity) {
        mPreviousEntity = currentEntity;
        mEntityChanged = true;
      }
      else
        mEntityChanged = false;

      // @TODO: EDIT WHEN NEW COMPONENTS (ALSO ITS OWN WINDOW FUNCTION) 

      if (currentEntity.HasComponent<Component::Tag>())
        TagComponentWindow(currentEntity, std::string(ICON_FA_TAG));

      if (currentEntity.HasComponent<Component::Collider>())
        ColliderComponentWindow(currentEntity, std::string(ICON_FA_BOMB));

      if (currentEntity.HasComponent<Component::Layer>())
        LayerComponentWindow(currentEntity, std::string(ICON_FA_LAYER_GROUP));

      if (currentEntity.HasComponent<Component::Material>())
        MaterialComponentWindow(currentEntity, std::string(ICON_FA_GEM));

      if (currentEntity.HasComponent<Component::Mesh>())
        MeshComponentWindow(currentEntity, std::string(ICON_FA_CUBE));

      if (currentEntity.HasComponent<Component::RigidBody>())
        RigidBodyComponentWindow(currentEntity, std::string(ICON_FA_CAR));

      if (currentEntity.HasComponent<Component::Script>())
        ScriptComponentWindow(currentEntity, std::string(ICON_FA_FILE_CODE));

      if (currentEntity.HasComponent<Component::Text>())
        TextComponentWindow(currentEntity, std::string(ICON_FA_FONT));

      if (currentEntity.HasComponent<Component::Transform>())
        TransformComponentWindow(currentEntity,std::string(ICON_FA_ROTATE));
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
        mIsComponentEdited = mFirstEdit = false;
      }
      else if (state == Events::SceneStateChange::NEW) {
        mIsComponentEdited = true;
        mFirstEdit = false;
      }
      break;
    }
    default: break;
    }
  }

  void Inspector::LayerComponentWindow(ECS::Entity entity, std::string const& icon) {
    bool isOpen{ WindowBegin<Component::Layer>("Layer", icon) };

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
            SetIsComponentEdited(true);
          }
        }
        ImGui::EndCombo();
      }

      ImGui::EndTable();
    }

    WindowEnd(isOpen);
  }

  void Inspector::MaterialComponentWindow(ECS::Entity entity, std::string const& icon) {
    bool isOpen{ WindowBegin<Component::Material>("Material", icon) };

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
      if (ImGui::ColorEdit4("##MaterialColor", &material.color[0])) {
        SetIsComponentEdited(true);
      }

      ImGui::EndTable();
    }

    WindowEnd(isOpen);
  }

  void Inspector::ScriptComponentWindow(ECS::Entity entity, std::string const& icon) {
    bool isOpen{ WindowBegin<Component::Script>("Script", icon) };

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
            SetIsComponentEdited(true);
          }
        }
        ImGui::EndCombo();
      }

      ImGui::EndTable();
    }

    WindowEnd(isOpen);
  }

  void Inspector::TagComponentWindow(ECS::Entity entity, std::string const& icon) {
    bool isOpen{ WindowBegin<Component::Tag>("Tag", icon) };

    if (isOpen) {
      std::string tag{ entity.GetTag() };
      ImGui::PushFont(mStyler.GetCustomFont(GUI::MONTSERRAT_SEMIBOLD));
      ImGui::SetNextItemWidth(INPUT_SIZE);
      if (ImGui::InputText("##Tag", &tag, ImGuiInputTextFlags_EnterReturnsTrue)) {
        entity.SetTag(tag);
        SetIsComponentEdited(true);
      }
      ImGui::PopFont();

      DrawAddButton();
    }

    WindowEnd(isOpen);
  }

  void Inspector::TextComponentWindow(ECS::Entity entity, std::string const& icon) {
    bool isOpen{ WindowBegin<Component::Text>("Text", icon) };

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
            SetIsComponentEdited(true);
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
        SetIsComponentEdited(true);
      }

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Text Input");
      ImGui::TableSetColumnIndex(1);
      ImGui::SetNextItemWidth(INPUT_SIZE);
      if (ImGui::InputTextMultiline("##TextInput", &text.textContent)) {
        SetIsComponentEdited(true);
      }

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Scale");
      ImGui::TableSetColumnIndex(1);
      ImGui::SetNextItemWidth(INPUT_SIZE);
      if (ImGui::DragFloat("##TextScale", &text.scale, .001f, 0.f, 5.f)) {
        SetIsComponentEdited(true);
      }

      ImGui::EndTable();
    }

    WindowEnd(isOpen);
  }

  void Inspector::TransformComponentWindow(ECS::Entity entity, std::string const& icon) {
    bool isOpen{ WindowBegin<Component::Transform>("Transform", icon) };

    if (isOpen) {
      auto& transform = entity.GetComponent<Component::Transform>();

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
      if (ImGuiHelpers::TableInputFloat3("Local Translation", &transform.localPos[0], inputWidth, false, -100.f, 100.f, 0.1f)) {
        SetIsComponentEdited(true);
      }
      if (ImGuiHelpers::TableInputFloat3("Local Rotation", &transform.localRot[0], inputWidth, false, 0.f, 360.f, 0.1f)) {
        SetIsComponentEdited(true);
      }
      if (ImGuiHelpers::TableInputFloat3("Local Scale", &transform.localScale[0], inputWidth, false, 0.f, 100.f, 1.f)) {
        SetIsComponentEdited(true);
      }
      ImGui::EndTable();

      ImGui::BeginTable("WorldTransformTable", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, FIRST_COLUMN_LENGTH);
      ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableHeadersRow();

      // @TODO: Replace min and max with the world min and max
      if (ImGuiHelpers::TableInputFloat3("World Translation", &transform.worldPos[0], inputWidth, false, -100.f, 100.f, 0.1f)) {
        SetIsComponentEdited(true);
      }
      if (ImGuiHelpers::TableInputFloat3("World Rotation", &transform.worldRot[0], inputWidth, false, 0.f, 360.f, 0.1f)) {
        SetIsComponentEdited(true);
      }
      if (ImGuiHelpers::TableInputFloat3("World Scale", &transform.worldScale[0], inputWidth, false, 0.f, 100.f, 1.f)) {
        SetIsComponentEdited(true);
      }

      ImGui::EndTable();
    }

    WindowEnd(isOpen);
  }

  void Inspector::MeshComponentWindow(ECS::Entity entity, std::string const& icon) {
    bool isOpen{ WindowBegin<Component::Mesh>("Mesh", icon) };

    if (isOpen) {

    }

    WindowEnd(isOpen);
  }

  void Inspector::WindowEnd(bool isOpen) {
    if (isOpen) {
      ImGui::TreePop();
      ImGui::PopFont();
    }

    ImGui::Separator();
  }

  void Inspector::RigidBodyComponentWindow(ECS::Entity entity, std::string const& icon) {
    bool isOpen{ WindowBegin<Component::RigidBody>("RigidBody", icon) };

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
        SetIsComponentEdited(true);
      }
      if (ImGuiHelpers::TableInputFloat3("Angular Velocity", rigidBody.angularVelocity.mF32, inputWidth, false, -100.f, 100.f, 0.1f)) {
        IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::ANGULAR_VELOCITY);
        SetIsComponentEdited(true);
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
        SetIsComponentEdited(true);
      }

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Restitution");
      ImGui::TableSetColumnIndex(1);
      ImGui::SetNextItemWidth(INPUT_SIZE);
      if (ImGui::DragFloat("##RigidBodyRestitution", &rigidBody.restitution, 0.01f, 0.0f, 1.0f)) {
        IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::RESTITUTION);
        SetIsComponentEdited(true);
      }

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Gravity Factor");
      ImGui::TableSetColumnIndex(1);
      ImGui::SetNextItemWidth(INPUT_SIZE);
      if (ImGui::DragFloat("##RigidBodyGravityFactor", &rigidBody.gravityFactor, 0.01f, 0.0f, 10.0f)) {
        IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::GRAVITY_FACTOR);
        SetIsComponentEdited(true);
      }

      // Motion Type Selection
      const char* motionTypes[] = { "Static", "Kinematic", "Dynamic" };
      int currentMotionType = static_cast<int>(rigidBody.motionType);

      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("Motion Type");
      ImGui::TableSetColumnIndex(1);
      ImGui::SetNextItemWidth(INPUT_SIZE);
      if (ImGui::Combo("##Motion Type", &currentMotionType, motionTypes, IM_ARRAYSIZE(motionTypes))) {
        rigidBody.motionType = static_cast<JPH::EMotionType>(currentMotionType);
        IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::MOTION);
        SetIsComponentEdited(true);
      }

      ImGui::EndTable();
    }

    WindowEnd(isOpen);
  }

  void Inspector::ColliderComponentWindow(ECS::Entity entity, std::string const& icon) {
    bool isOpen{ WindowBegin<Component::Collider>("Collider", icon) };

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
        SetIsComponentEdited(true);
      }
      if (ImGuiHelpers::TableInputFloat3("Position Offset", collider.positionOffset.mF32, inputWidth, false, -100.f, 100.f, 0.1f)) {
        SetIsComponentEdited(true);
      }
      if (ImGuiHelpers::TableInputFloat3("Rotation Offset", collider.rotationOffset.mF32, inputWidth, false, 0.f, 360.f, 0.1f)) {
        SetIsComponentEdited(true);
      }

      ImGui::EndTable();

      // Shape Type Selection
      const char* shapeTypes[] = { "Unknown", "Sphere", "Capsule", "Box", "Triangle", "ConvexHull", "Mesh", "HeightField", "Compound" };
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
        SetIsComponentEdited(true);
      }

      ImGui::EndTable();
    }

    WindowEnd(isOpen);
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

#endif  // IMGUI_DISABLE