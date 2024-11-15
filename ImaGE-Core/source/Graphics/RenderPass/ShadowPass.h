#pragma once
#include "RenderPass.h"
#include <glm/glm.hpp>
#include <Core/Entity.h>

namespace Component { struct Light; struct Camera; }

namespace Graphics {
  class ShadowPass : public RenderPass {
  public:
    ShadowPass(const RenderPassSpec& spec);

    bool IsActive() const noexcept { return mActive; }
    void Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities) override;

    uint32_t BindShadowMap();

    // @TODO: find a better way to retrive uniforms instead of getter for each one
    inline glm::mat4 const& GetLightSpaceMatrix() const noexcept { return mLightSpaceMtx; }
    inline float GetShadowBias() const noexcept { return mShadowBias; }
    inline int GetShadowSoftness() const noexcept { return mShadowSoftness; }
    inline uint32_t GetShadowMapBuffer() const { return GetTargetFramebuffer()->GetDepthAttachmentID(); }

  private:
    void Init();
    bool LocateLightEntity();
    void ComputeLightSpaceMatrix(Component::Light& light, glm::quat const& lightRot);

    void StartRender();
    void EndRender();

    void SetLightUniforms();
    
    // temp
    // @TODO: have to account for multiple lights in future
    static inline float sDefaultSceneBounds = 50.f;

    glm::mat4 mLightSpaceMtx;
    ECS::Entity mLightEntity;
    int mShadowSoftness;
    float mShadowBias;

    bool mActive;
  };

} // namespace Graphics
