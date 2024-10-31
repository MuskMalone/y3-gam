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

    uint32_t BindShadowMap();
    inline glm::mat4 const& GetLightSpaceMatrix() const noexcept { return mLightSpaceMtx; }
    inline float GetShadowBias() const noexcept { return mShadowBias; }
    inline int GetShadowSoftness() const noexcept { return mShadowSoftness; }
    inline uint32_t GetShadowMapBuffer() const { return GetTargetFramebuffer()->GetDepthAttachmentID(); }

  private:

    bool LocateLightSource(CameraSpec const& cam, std::vector<ECS::Entity> const& entities); //edit tmp redundamncy
    void StartRender();
    void EndRender();

    void SetLightUniforms(CameraSpec const& cam, glm::vec3 const& lightDir, float nearPlaneMultiplier, glm::vec3 test);
    
    // temp
    // @TODO: have to account for multiple lights in future
    glm::mat4 mLightSpaceMtx;
    int mShadowSoftness;
    float mShadowBias;

    bool mActive;
  };

} // namespace Graphics
