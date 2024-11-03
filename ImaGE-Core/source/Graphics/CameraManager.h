#pragma once
#include "Core/Entity.h"
#include "Core/Components/Components.h"

namespace Graphics {
    class CameraManager {
    public:
        void AddCamera(const ECS::Entity& cameraEntity) {
            cameras.push_back(cameraEntity);
        }

        void SetActiveCamera(int index) {
            if (index >= 0 && index < cameras.size()) {
                activeCameraIndex = index;
            }
        }

        const ECS::Entity& GetActiveCamera() const {
            return cameras[activeCameraIndex];
        }

        const Component::Camera& GetActiveCameraComponent() const {
            return GetActiveCamera().GetComponent<Component::Camera>();
        }


        bool HasActiveCamera() const {
            return !cameras.empty();
        }
        

    private:
        std::vector<ECS::Entity> cameras;
        int activeCameraIndex{ 0 }; // Default to the first camera
    };

}