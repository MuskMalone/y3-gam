#include <pch.h>
#ifndef IMGUI_DISABLE
#include <imgui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include "Inspector.h"
#include <Physics/PhysicsSystem.h>
#include <TempScene.h> //tch for testing to remove
#include <functional>
namespace GUI {
  // Static Initialization
  std::map<std::string, bool> Inspector::sComponentOpenStatusMap{};
  bool Inspector::sFirstOpen{ true };
  ECS::Entity Inspector::sPreviousEntity{};
  bool Inspector::sEntityChanged{};

  Inspector::Inspector(std::string const& name) : GUIWindow(name) {}

  std::vector<std::pair<std::string, std::function<void(ECS::Entity)>>> componentNames = {
    { "Tag", [&](ECS::Entity) {
    }},
    { "Layer", [&](ECS::Entity) {
    }},
    { "Transform", [&](ECS::Entity) {

    }},
    { "Mesh",[&](ECS::Entity currentEntity) {
        Scene::AddMesh(currentEntity);
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
    if (sFirstOpen) {
      for (std::string const& component : Component::ComponentNameList) {
        sComponentOpenStatusMap[component] = false;
      }
      sFirstOpen = false;
    }

    ImGui::Begin(mWindowName.c_str());
    ShowAddComponentPopup();
    ECS::Entity const& currentEntity{ GUIManager::GetSelectedEntity() };
    if (currentEntity) {
      if (currentEntity != sPreviousEntity) {
        sPreviousEntity = currentEntity;
        sEntityChanged = true;
      }
      else
        sEntityChanged = false;

      if (currentEntity.HasComponent<Component::Tag>())
        TagComponentWindow(currentEntity);

      if (currentEntity.HasComponent<Component::Layer>())
        LayerComponentWindow(currentEntity);

      if (currentEntity.HasComponent<Component::Transform>())
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
            ImGui::DragFloat3("World Position", &xfm.worldPos.x, 0.1f);
            ImGui::DragFloat3("World Scale", &xfm.worldScale.x, 0.1f);
            ImGui::DragFloat3("World Rotation", &xfm.worldRot.x, 0.1f);
        }
    }

    WindowEnd(isOpen);
  }

  bool Inspector::WindowBegin(std::string windowName) {
    ImGui::Separator();
    if (sEntityChanged) {
      bool& openMapStatus = sComponentOpenStatusMap[windowName];
      ImGui::SetNextItemOpen(openMapStatus, ImGuiCond_Always);
    }

    bool isOpen{ ImGui::TreeNode(windowName.c_str()) };
    sComponentOpenStatusMap[windowName] = isOpen;
    return isOpen;
  }

  void Inspector::WindowEnd(bool isOpen) {
    if (isOpen)
      ImGui::TreePop();

    ImGui::Separator();
  void Inspector::RigidBodyComponentWindow(ECS::Entity entity) {
      ImGui::Separator();
      Component::RigidBody& rigidBody{entity.GetComponent<Component::RigidBody>()};
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
