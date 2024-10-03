#include <pch.h>
#include "RenderPass.h"
#include "Core/Entity.h"
#include "EditorCamera.h"
#include "Core/Component/Mesh.h"

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
    }

    void RenderPass::Render(EditorCamera const& cam, std::vector<ECS::Entity> const& entities) {

        Begin();

        auto shader = mSpec.pipeline->GetShader();
        shader->SetUniform("u_ViewProjMtx", cam.GetViewProjMatrix());
        shader->SetUniform("u_CamPos", cam.GetPosition());
        shader->SetUniform("u_Albedo", glm::vec3(1.0, 0.5, 0.31));
        shader->SetUniform("u_Metallic", 0.0f);
        shader->SetUniform("u_Roughness", 0.3f);
        shader->SetUniform("u_AO", 1.f);

        //@TODO in future add light + materials
        
        //Renderer::BeginBatch();
        for (auto const& entity : entities) {
            // Extract Transform and Mesh components
            if (!entity.HasComponent<Component::Transform>() || !entity.HasComponent<Component::Mesh>())
                continue;

            auto const& xform = entity.GetComponent<Component::Transform>();
            auto const& mesh = entity.GetComponent<Component::Mesh>();
            
            xform.localPos; //example xform.localPos

            
            // Skip if mesh is null
            if (mesh.mesh == nullptr)
                continue;

            // Submit the mesh to the renderer with transform information
            //Graphics::Renderer::SubmitMesh(mesh.mesh, xform.worldPos, xform.worldRot, xform.worldScale, { 1.f, 1.f, 1.f, 1.f }); //@TODO: adjust color and rotation as needed
            Graphics::Renderer::SubmitInstance(mesh.mesh, xform.worldPos, xform.worldRot, xform.worldScale, { 1.f,1.f,1.f,1.f });
        }
        // Flush all collected instances and render them in a single draw call
        Renderer::RenderInstances();
        //Renderer::FlushBatch(shared_from_this());

        End();
    }

    std::shared_ptr<RenderPass> RenderPass::Create(const RenderPassSpec& spec) {
        return std::make_shared<RenderPass>(spec);
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