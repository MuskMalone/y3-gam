#include <pch.h>
#include <imgui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include "Inspector.h"
#include <Physics/PhysicsSystem.h>
#include <functional>
namespace GUI {
  Inspector::Inspector(std::string const& name) : GUIWindow(name) {}

  std::vector<std::pair<std::string, std::function<void(ECS::Entity)>>> componentNames = {
    { "Tag", [&](ECS::Entity) {
    }},
    { "Layer", [&](ECS::Entity) {
    }},
    { "Transform", [&](ECS::Entity) {

    }},
    { "Mesh",[&](ECS::Entity) {

    }},
    { "RigidBody", [&](ECS::Entity currentEntity) {
        IGE::Physics::PhysicsSystem::GetInstance()->AddRigidBody(currentEntity);
    }},
    { "Collider", [&](ECS::Entity currentEntity) {
        IGE::Physics::PhysicsSystem::GetInstance()->AddCollider(currentEntity);
    }},
  };
  // Function to show the "Add Component" button and component selection
  void ShowAddComponentPopup() {
      // List of component names (could also use an enum or type list for better type safety)
      ECS::Entity const& currentEntity{ GUIManager::GetSelectedEntity() };

      // Button to open the popup
      if (ImGui::Button("Add Component")) {
          ImGui::OpenPopup("AddComponentPopup");
      }

      // Popup window
      if (ImGui::BeginPopup("AddComponentPopup")) {
          // List of components
          for (int i = 0; i < componentNames.size(); i++) {
              if (ImGui::Selectable(componentNames[i].first.c_str())) {
                  if (currentEntity) {
                      componentNames[i].second(currentEntity);
                  }
              }
          }
          ImGui::EndPopup();
      }
  }

  void Inspector::Run() {
    ImGui::Begin(mWindowName.c_str());
    ShowAddComponentPopup();
    ECS::Entity const& currentEntity{ GUIManager::GetSelectedEntity() };
    if (currentEntity) {
      if (currentEntity.HasComponent<Component::Tag>()) {
        TagComponentWindow(currentEntity);
      }

      if (currentEntity.HasComponent<Component::Layer>()) {
        LayerComponentWindow(currentEntity);
      }

      if (currentEntity.HasComponent<Component::Transform>()) {
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
    ImGui::Separator();
    std::string tag{ entity.GetTag() };
    if (ImGui::InputText("##Tag", &tag, ImGuiInputTextFlags_EnterReturnsTrue)) {
      entity.SetTag(tag);
    }
    ImGui::Separator();
  }

  void Inspector::LayerComponentWindow(ECS::Entity entity) {
    ImGui::Separator();

    ImGui::Separator();
  }

  void Inspector::TransformComponentWindow(ECS::Entity entity) {
    ImGui::Separator();

    ImGui::Separator();
  }

  void Inspector::RigidBodyComponentWindow(ECS::Entity entity) {
      ImGui::Separator();
      Component::RigidBody rigidBody{entity.GetComponent<Component::RigidBody>()};
      // Assuming 'rigidBody' is an instance of RigidBody

      ImGui::Checkbox("Use Gravity", &rigidBody.useGravity);

      ImGui::DragFloat("Friction", &rigidBody.friction, 0.01f, 0.0f, 1.0f);
      ImGui::DragFloat("Restitution", &rigidBody.restitution, 0.01f, 0.0f, 1.0f);
      ImGui::DragFloat("Gravity Factor", &rigidBody.gravityFactor, 0.01f, 0.0f, 10.0f);

      ImGui::DragFloat3("Velocity", rigidBody.velocity.mF32, 0.1f);
      ImGui::DragFloat3("Angular Velocity", rigidBody.angularVelocity.mF32, 0.1f);

      // Motion Type Selection
      const char* motionTypes[] = { "Static", "Dynamic", "Kinematic" };
      int currentMotionType = static_cast<int>(rigidBody.motionType);
      if (ImGui::Combo("Motion Type", &currentMotionType, motionTypes, IM_ARRAYSIZE(motionTypes))) {
          rigidBody.motionType = static_cast<JPH::EMotionType>(currentMotionType);
      }
  }

  void Inspector::ColliderComponentWindow(ECS::Entity entity) {
      ImGui::Separator();
      // Assuming 'collider' is an instance of Collider
      Component::Collider collider{entity.GetComponent<Component::Collider>()};
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
