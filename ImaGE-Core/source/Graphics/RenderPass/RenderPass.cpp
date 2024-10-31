#include <pch.h>
#include "RenderPass.h"
#include <Graphics/Texture.h>

namespace Graphics {
    RenderPass::RenderPass(const RenderPassSpec& spec) : mSpec(spec) {}

    RenderPassSpec& RenderPass::GetSpecification() {
      return mSpec;
    }

    RenderPassSpec const& RenderPass::GetSpecification() const {
      return mSpec;
    }

    std::shared_ptr<Pipeline> RenderPass::GetPipeline() const {
      return mSpec.pipeline;
    }

    std::shared_ptr<Framebuffer> RenderPass::GetTargetFramebuffer() const {
      return mSpec.pipeline->GetSpec().targetFramebuffer;
    }

    //bool Validate() { return true; }

    void RenderPass::Begin() {
      mSpec.pipeline->Bind();
    }

    void RenderPass::End() {
      mSpec.pipeline->Unbind();
      Texture::ResetTextureUnits();
    }

    //std::shared_ptr<Image2D> RenderPass::GetOutput(uint32_t index)
    //{
    //    std::shared_ptr<Framebuffer> framebuffer = mSpec.pipeline->GetSpec().targetFramebuffer;
    //    if (index > framebuffer->GetColorAttachmentCount() + 1)
    //        return nullptr; // Invalid index
    //    if (index < framebuffer->GetColorAttachmentCount())
    //        return framebuffer->GetImage(index);
    //    return framebuffer->GetDepthImage();
    //}
    
    //GetDepthOutput...
}