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
    void Render(EditorCamera const& camera, std::vector<ECS::Entity> const& entities) override;

    int BindShadowMap();
    inline glm::mat4 const& GetLightSpaceMatrix() const { return mLightSpaceMtx; }
    inline uint32_t GetShadowMapBuffer() const { return GetTargetFramebuffer()->GetDepthAttachmentID(); }

  private:
    bool LocateLightSource(EditorCamera const& cam, std::vector<ECS::Entity> const& entities);
    void StartRender();
    void EndRender();

    void SetLightUniforms(EditorCamera const& cam, glm::vec3 const& lightDir, float nearPlaneMultiplier, glm::vec3 test);
    
    glm::mat4 mLightSpaceMtx;
    bool mActive;
  };

} // namespace Graphics
