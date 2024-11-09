#pragma once
#include "Core/Entity.h"
#include "Core/Components/Components.h"
#include "Core/EntityManager.h"

namespace Graphics {
    class CameraManager {
    public:
        void AddCamera() {
            ECS::Entity cameraEntity = ECS::EntityManager::GetInstance().CreateEntity();
            auto& camComp = cameraEntity.EmplaceComponent<Component::Camera>();
        }

        void AddMainCamera() {
            ECS::Entity mainCameraEntity = ECS::EntityManager::GetInstance().CreateEntity();
            auto& camCom = mainCameraEntity.EmplaceComponent<Component::Camera>();
            camCom.fov = 60.f;

            mainCameraEntity.SetTag("MainCamera");
        }

        void SetActiveCamera(int index) {
            //if (index >= 0 && index < cameras.size()) {
            //    activeCameraIndex = index;
            //}
            auto cameras = ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::Camera>();
            if (index >= 0 && index < cameras.size()) {
                activeCameraIndex = index;
            }
        }

        ECS::Entity GetActiveCamera() {
            // First, try to find the camera tagged "MainCamera"
            auto cameras = ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::Camera>();
            for (auto& cam : cameras) {
                if (ECS::Entity{ cam }.GetTag() == "MainCamera") {
                    return cam; 
                }
            }

            // If no "MainCamera" tag is found, default to the camera at activeCameraIndex
            return cameras[activeCameraIndex];
        }

        Component::Camera& GetActiveCameraComponent()  {
            // Retrieve the active camera entity
            auto activeCamera = GetActiveCamera();

            // Get both the Camera and Transform components
            Component::Camera& cameraComp = activeCamera.GetComponent<Component::Camera>();
            const auto& transformComp = activeCamera.GetComponent<Component::Transform>();
            // Update the Camera component’s values with Transform data
            cameraComp.position = transformComp.worldPos;

            cameraComp.rotation = transformComp.worldRot;


            return cameraComp;
            
            //return GetActiveCamera().GetComponent<Component::Camera>();
        }


        bool HasActiveCamera() const {
            auto cameras = ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::Camera>();
            return !cameras.empty();
        }
        
    private:
        int activeCameraIndex{ 0 };
    };

}