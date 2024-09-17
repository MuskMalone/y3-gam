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

namespace GUI {

  //Static Initialization
  bool Inspector::sIsComponentEdited{};

  template<typename Component>
  void DrawAddComponentButton(std::string const& name);

  template<typename Component>
  bool DrawOptionButton(std::string const& name);

  template<typename Component>
  bool DrawOptionsListButton(std::string windowName);

  Inspector::Inspector(std::string const& name) : GUIWindow(name),
    mComponentOpenStatusMap{}, mObjFactory{ Reflection::ObjectFactory::GetInstance() },
    mPreviousEntity{}, mEntityChanged{ false } {
    for (std::string const& component : Component::ComponentNameList) {
      mComponentOpenStatusMap[component] = true; // Default to all open
    }
  }

  void Inspector::Run() {
    ImGui::Begin(mWindowName.c_str());
    ECS::Entity const& currentEntity{ GUIManager::GetSelectedEntity() };
    
    if (currentEntity) {

      if (currentEntity != mPreviousEntity) {
        mPreviousEntity = currentEntity;
        mEntityChanged = true;
      }
      else
        mEntityChanged = false;

      // @TODO: EDIT WHEN NEW COMPONENTS
      if (currentEntity.HasComponent<Component::Tag>())
        TagComponentWindow(currentEntity);

      if (currentEntity.HasComponent<Component::Collider>())
        ColliderComponentWindow(currentEntity);

      if (currentEntity.HasComponent<Component::Layer>())
        LayerComponentWindow(currentEntity);

      if (currentEntity.HasComponent<Component::Material>())
        MaterialComponentWindow(currentEntity);

      if (currentEntity.HasComponent<Component::Mesh>())
        MeshComponentWindow(currentEntity);

      if (currentEntity.HasComponent<Component::RigidBody>())
        RigidBodyComponentWindow(currentEntity);

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

  void Inspector::MaterialComponentWindow(ECS::Entity entity) {
    bool isOpen{ WindowBegin<Component::Material>("Material") };

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
      float firstColumnCharSize = ImGui::CalcTextSize("Local Translation").x;
      float inputWidth = (contentSize - charSize - 60) / 3;

      ImGui::BeginTable("LocalTransformTable", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, firstColumnCharSize);
      ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableHeadersRow();

      if (ImGuiHelpers::TableInputDouble3("Local Translation", transform.localPos, inputWidth, false)) {
        SetIsComponentEdited(true);
      }
      if (ImGuiHelpers::TableInputDouble3("Local Rotation", transform.localRot, inputWidth, false)) {
        SetIsComponentEdited(true);
      }
      if (ImGuiHelpers::TableInputDouble3("Local Scale", transform.localScale, inputWidth, false)) {
        SetIsComponentEdited(true);
      }
      ImGui::EndTable();

      ImGui::BeginTable("WorldTransformTable", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit);

      ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, firstColumnCharSize);
      ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthFixed, inputWidth);
      ImGui::TableHeadersRow();

      if (ImGuiHelpers::TableInputDouble3("World Translation", transform.worldPos, inputWidth, true)) {
        SetIsComponentEdited(true);
      }
      if (ImGuiHelpers::TableInputDouble3("World Rotation", transform.worldRot, inputWidth, true)) {
        SetIsComponentEdited(true);
      }
      if (ImGuiHelpers::TableInputDouble3("World Scale", transform.worldScale, inputWidth, true)) {
        SetIsComponentEdited(true);
      }

      ImGui::EndTable();
    }

    WindowEnd(isOpen);
  }

  void Inspector::MeshComponentWindow(ECS::Entity entity) {
    bool isOpen{ WindowBegin<Component::Mesh>("Mesh") };

    if (isOpen) {

    }

    WindowEnd(isOpen);
  }

  void Inspector::WindowEnd(bool isOpen) {
    if (isOpen)
        ImGui::TreePop();

    ImGui::Separator();
  }

  void Inspector::RigidBodyComponentWindow(ECS::Entity entity) {
    bool isOpen{ WindowBegin<Component::RigidBody>("RigidBody") };

    if (isOpen) {
      Component::RigidBody& rigidBody{ entity.GetComponent<Component::RigidBody>() };
      // Assuming 'rigidBody' is an instance of RigidBody
      ImGui::DragFloat("Friction", &rigidBody.friction, 0.01f, 0.0f, 1.0f);
      ImGui::DragFloat("Restitution", &rigidBody.restitution, 0.01f, 0.0f, 1.0f);
      ImGui::DragFloat("Gravity Factor", &rigidBody.gravityFactor, 0.01f, 0.0f, 10.0f);

      ImGui::DragFloat3("Velocity", rigidBody.velocity.mF32, 0.1f);
      ImGui::DragFloat3("Angular Velocity", rigidBody.angularVelocity.mF32, 0.1f);

      // Motion Type Selection
      const char* motionTypes[] = { "Static", "Kinematic", "Dynamic" };
      int currentMotionType = static_cast<int>(rigidBody.motionType);
      if (ImGui::Combo("Motion Type", &currentMotionType, motionTypes, IM_ARRAYSIZE(motionTypes))) {
        rigidBody.motionType = static_cast<JPH::EMotionType>(currentMotionType);
        IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::MOTION);
      }
    }

    WindowEnd(isOpen);
  }

  void Inspector::ColliderComponentWindow(ECS::Entity entity) {
    bool isOpen{ WindowBegin<Component::RigidBody>("RigidBody") };

    if (isOpen) {
      // Assuming 'collider' is an instance of Collider
      Component::Collider& collider{ entity.GetComponent<Component::Collider>() };
      ImGui::DragFloat3("Scale", collider.scale.mF32, 0.1f);
      ImGui::DragFloat3("Position Offset", collider.positionOffset.mF32, 0.1f);
      ImGui::DragFloat3("Rotation Offset", collider.rotationOffset.mF32, 0.1f);

      // Shape Type Selection
      const char* shapeTypes[] = { "Unknown", "Sphere", "Capsule", "Box", "Triangle", "ConvexHull", "Mesh", "HeightField", "Compound" };
      int currentShapeType = static_cast<int>(collider.type);
      if (ImGui::Combo("Shape Type", &currentShapeType, shapeTypes, IM_ARRAYSIZE(shapeTypes))) {
        collider.type = static_cast<JPH::EShapeSubType>(currentShapeType);
      }
    }

    WindowEnd(isOpen);
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
        DrawAddComponentButton<Component::Material>("Material");
        DrawAddComponentButton<Component::Mesh>("Mesh");
        DrawAddComponentButton<Component::RigidBody>("RigidBody");
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
  bool DrawOptionButton(std::string const& name) {
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
        openMainWindow = false;
      }

      else if (name == "Clear") {
        auto& component = ent.GetComponent<Component>();
        component.Clear();
      }

      ImGui::CloseCurrentPopup();
    }

    return openMainWindow;
  }

  template<typename Component>
  bool DrawOptionsListButton(std::string windowName) {
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
} // namespace GUI

#endif  // IMGUI_DISABLE