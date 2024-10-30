#pragma once
#include "RenderPass.h"
#include <glm/glm.hpp>
namespace Component { struct Light; }

namespace Graphics {
  class EditorCam;

  class ShadowPass : public RenderPass {
  public:
    ShadowPass(const RenderPassSpec& spec);

    bool IsActive() const noexcept { return mActive; }
    void Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities) override;
    void Render(EditorCamera const& camera, std::vector<ECS::Entity> const& entities) override;

  private:
      bool SetLightUniforms(CameraSpec const& cam, std::vector<ECS::Entity> const& entities); //edit tmp redundamncy
    bool SetLightUniforms(EditorCamera const& cam, std::vector<ECS::Entity> const& entities);
    void StartRender();
    void EndRender();

    std::pair<glm::vec3, glm::vec3> GetLightProjPlanes(EditorCamera const& cam, glm::vec3 const& lightPos, glm::vec3 const& lightDir);
    std::pair<glm::vec3, glm::vec3> GetLightProjPlanes(CameraSpec const& cam, glm::vec3 const& lightPos, glm::vec3 const& lightDir); //edit tmp redundancy
    
    bool mActive;
  };

} // namespace Graphics
