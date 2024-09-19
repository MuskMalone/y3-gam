#include <pch.h>
#ifndef IMGUI_DISABLE
#include <imgui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include "Inspector.h"
#include <Physics/PhysicsSystem.h>
#include <functional>
#include <Reflection/ComponentTypes.h>

namespace GUI {
  Inspector::Inspector(std::string const& name) : GUIWindow(name),
    mComponentOpenStatusMap{}, mObjFactory{ Reflection::ObjectFactory::GetInstance() },
    mPreviousEntity{}, mFirstOpen{ true }, mEntityChanged{ false } {}

  // Function to show the "Add Component" button and component selection
  void Inspector::ShowAddComponentPopup() {
      // Popup window
      if (ImGui::BeginPopup("AddComponentPopup")) {
        // Iterate through vector of rttr::types and add components via reflection
        ImGui::Text("Components");
        if (ImGui::BeginCombo("##Components", Reflection::gComponentTypes.front().get_name().to_string().c_str()))
        {
          for (int i = 0; i < Reflection::gComponentTypes.size(); ++i) {

            // for now this is automated, but if we want to add QOL changes
            // like preventing existing components to be added, we need to break this up
            // similarly for deleting components, we can use ObjectFactory::RemoveComponentFromEntity
            /*
            Example:
            if (ImGui::Selectable(Reflection::gComponentTypes[i].get_name().to_string().c_str())) {
              if (Reflection::gComponentTypes[i] == rttr::type::get<Component::Tag>) {...}
              else if (Reflection::gComponentTypes[i] == rttr::type::get<Component::Layer>) {...}
              else if (Reflection::gComponentTypes[i] == rttr::type::get<Component::Collider>) {...}
              else if (Reflection::gComponentTypes[i] == rttr::type::get<Component::RigidBody>) {...}
            }
            */
            if (ImGui::Selectable(Reflection::gComponentTypes[i].get_name().to_string().c_str())) {
              mObjFactory.AddComponentToEntity(GUIManager::GetSelectedEntity(), Reflection::gComponentTypes[i]);
            }

          }
          ImGui::EndCombo();
        }
        ImGui::EndPopup();
      }
  }

  void Inspector::Run() {
    if (mFirstOpen) {
      for (std::string const& component : Component::ComponentNameList) {
        mComponentOpenStatusMap[component] = false;
      }
      mFirstOpen = false;
    }

    ImGui::Begin(mWindowName.c_str());

    if (GUIManager::GetSelectedEntity()) {
      // Button to open the popup
      if (ImGui::Button("Add Component")) {
        ImGui::OpenPopup("AddComponentPopup");
      }
    }
    ShowAddComponentPopup();

    ECS::Entity const& currentEntity{ GUIManager::GetSelectedEntity() };
    if (currentEntity) {
      if (currentEntity != mPreviousEntity) {
        mPreviousEntity = currentEntity;
        mEntityChanged = true;
      }
      else
        mEntityChanged = false;

      if (currentEntity.HasComponent<Component::Tag>())
        TagComponentWindow(currentEntity);

      if (currentEntity.HasComponent<Component::Layer>())
        LayerComponentWindow(currentEntity);

      if (currentEntity.HasComponent<Component::Transform>()){
        TransformComponentWindow(currentEntity);
      }

      if (currentEntity.HasComponent<Component::Mesh>()) {
      
      }

      if (currentEntity.HasComponent<Component::RigidBody>()) {
          RigidBodyComponentWindow(currentEntity);
      }

      if (currentEntity.HasComponent<Component::Collider>()) {
          ColliderComponentWindow(currentEntity);
      }

    }

    ImGui::End();
  }

  void Inspector::TagComponentWindow(ECS::Entity entity) {
    bool isOpen{ WindowBegin("Tag") };

    if (isOpen) {
      std::string tag{ entity.GetTag() };
      if (ImGui::InputText("##Tag", &tag, ImGuiInputTextFlags_EnterReturnsTrue)) {
        entity.SetTag(tag);
      }
    }

    WindowEnd(isOpen);
  }

  void Inspector::LayerComponentWindow(ECS::Entity entity) {
    bool isOpen{ WindowBegin("Layer") };

    if (isOpen) {
      
    }

    WindowEnd(isOpen);
  }

  void Inspector::TransformComponentWindow(ECS::Entity entity) {
    bool isOpen{ WindowBegin("Transform") };

    if (isOpen) {
        { // tch added for testing, pls remove if needed
            auto& xfm{ entity.GetComponent<Component::Transform>() };
            ImGui::DragScalarN("World Position", ImGuiDataType_Double, &xfm.worldPos.x, 3, 0.1f);
            ImGui::DragScalarN("World Scale", ImGuiDataType_Double, &xfm.worldScale.x, 3, 0.1f);
            ImGui::DragScalarN("World Rotation", ImGuiDataType_Double, &xfm.worldRot.x, 3, 0.1f);
        }
    }

    WindowEnd(isOpen);
  }

  bool Inspector::WindowBegin(std::string windowName) {
    ImGui::Separator();
    if (mEntityChanged) {
      bool& openMapStatus = mComponentOpenStatusMap[windowName];
      ImGui::SetNextItemOpen(openMapStatus, ImGuiCond_Always);
    }

    bool isOpen{ ImGui::TreeNode(windowName.c_str()) };
    mComponentOpenStatusMap[windowName] = isOpen;
    return isOpen;
  }

  void Inspector::WindowEnd(bool isOpen) {
      if (isOpen)
          ImGui::TreePop();

      ImGui::Separator();

  }
  void Inspector::RigidBodyComponentWindow(ECS::Entity entity) {
      ImGui::Separator();
      Component::RigidBody& rigidBody{entity.GetComponent<Component::RigidBody>()};
      // Assuming 'rigidBody' is an instance of RigidBody

      if (ImGui::DragFloat("Friction", &rigidBody.friction, 0.01f, 0.0f, 1.0f)) {
          IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::FRICTION);
      }
      if (ImGui::DragFloat("Restitution", &rigidBody.restitution, 0.01f, 0.0f, 1.0f)) {
          IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::RESTITUTION);
      }

      if (ImGui::DragFloat("Gravity Factor", &rigidBody.gravityFactor, 0.01f, 0.0f, 10.0f)){
          IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::GRAVITY_FACTOR);
      }

      if (ImGui::DragFloat3("Velocity", rigidBody.velocity.mF32, 0.1f)){
          IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::VELOCITY);
      }
      if (ImGui::DragFloat3("Angular Velocity", rigidBody.angularVelocity.mF32, 0.1f)) {
          IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::ANGULAR_VELOCITY);
      }

      // Motion Type Selection
      const char* motionTypes[] = { "Static", "Kinematic", "Dynamic" };
      int currentMotionType = static_cast<int>(rigidBody.motionType);
      if (ImGui::Combo("Motion Type", &currentMotionType, motionTypes, IM_ARRAYSIZE(motionTypes))) {
          rigidBody.motionType = static_cast<JPH::EMotionType>(currentMotionType);
          IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::MOTION);
      }
  }

  void Inspector::ColliderComponentWindow(ECS::Entity entity) {
      ImGui::Separator();
      // Assuming 'collider' is an instance of Collider
      Component::Collider& collider{entity.GetComponent<Component::Collider>()};
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
} // namespace GUI

#endif  // IMGUI_DISABLE
