#include <pch.h>
#include "RenderPass.h"
#include "Core/Entity.h"
#include "EditorCamera.h"
#include <Core/Components/Components.h>
#include "Color.h"
#include "Asset/IGEAssets.h"
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

        //@TODO in future add light + materials
       
        //Renderer::BeginBatch();
        for (auto const& entity : entities) {
            if (!entity.HasComponent<Component::Transform>() || !entity.HasComponent<Component::Mesh>())
                continue;

            auto const& xform = entity.GetComponent<Component::Transform>();
            auto const& mesh = entity.GetComponent<Component::Mesh>();
            
            // Skip if mesh is null
            if (mesh.mesh == nullptr)
                continue;

            std::vector<GLuint64> hdls;
            for (auto const& map : Renderer::GetAlbedoMaps()) {
                hdls.push_back(IGE_REF(IGE::Assets::TextureAsset, map)->mTexture.GetBindlessHandle()); //should not be here TODO move somewhere else
            }
            //if (hdls.size() > 0)

            shader->SetUniform("u_AlbedoMap", IGE_REF(IGE::Assets::TextureAsset, Renderer::GetDefaultTexture())->mTexture, 0 );
            

            hdls.clear();
            for (auto const& map : Renderer::GetNormalMaps()) {
                hdls.push_back(IGE_REF(IGE::Assets::TextureAsset, map)->mTexture.GetBindlessHandle()); //should not be here TODO move somewhere else
            }
            //if (hdls.size() > 0)
            //    shader->SetUniform("u_NormalMap", hdls.data(), static_cast<unsigned int>(hdls.size()));

            uint32_t matID = 0;
            if (entity.HasComponent<Component::Material>()) {
                auto const& matComponent = entity.GetComponent<Component::Material>();
                //auto const& mat = matComponent.material;
                matID = matComponent.matIdx;
                shader->SetUniform("u_Albedo", glm::vec3(0.5, 0.5, 0.5));
                shader->SetUniform("u_Metalness", 0.0f);
                shader->SetUniform("u_Roughness", 0.0f);
                shader->SetUniform("u_Transparency", 1.0f);
                shader->SetUniform("u_AO", 1.f);
                //TEMP CODE

                //if (mat) {
                //    mat->Apply(shader);
                //}
            }
            else {
                shader->SetUniform("u_Albedo", glm::vec3(0.5,0.5,0.5));
                shader->SetUniform("u_Metalness", 0.0f);
                shader->SetUniform("u_Roughness", 0.0f);
                shader->SetUniform("u_Transparency", 1.0f);
                shader->SetUniform("u_AO", 1.f);
                //shader->SetUniform("u_AlbedoMap", Renderer::GetWhiteTexture(), 0);
            }

            //Graphics::Renderer::SubmitMesh(mesh.mesh, xform.worldPos, xform.worldRot, xform.worldScale, { 1.f, 1.f, 1.f, 1.f }); //@TODO: adjust color and rotation as needed
            Graphics::Renderer::SubmitInstance(mesh.mesh, xform.worldMtx, Color::COLOR_WHITE, hdls[1], entity.GetEntityID());
        }
        
        mSpec.pipeline->GetSpec().instanceLayout;
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