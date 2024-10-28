#include <pch.h>
#include "RenderPass.h"
#include "Core/Entity.h"
#include "EditorCamera.h"
#include <Core/Components/Components.h>
#include "Color.h"
#include "Asset/IGEAssets.h"
#include "MaterialTable.h"
namespace Graphics {

    using EntityXform = std::pair<ECS::Entity, glm::mat4>;
    using MatGroup = std::vector<EntityXform>;
    using MatGroupsMap = std::unordered_map<uint32_t, MatGroup>;
    
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
        //auto shader = mSpec.pipeline->GetShader();

        // Use a map to group entities by material ID
        MatGroupsMap matGroups;

        for (auto const& entity : entities) {
          if (!entity.HasComponent<Component::Transform, Component::Mesh>() || !entity.IsActive())
              continue;

            //auto const& xform = entity.GetComponent<Component::Transform>();
            auto const& mesh = entity.GetComponent<Component::Mesh>();

            // Skip if mesh is null
            if (mesh.mesh == nullptr)
                continue;

            uint32_t matID = 0;
            if (entity.HasComponent<Component::Material>()) {
                auto const& matComponent = entity.GetComponent<Component::Material>();
                matID = matComponent.matIdx;
            }
            auto const& xform = entity.GetComponent<Component::Transform>();
            matGroups[matID].emplace_back(entity, xform.worldMtx);
        //@TODO in future add light + materials
     
        }

        // Now render each material group
        for (const auto& [matID, entityPairs] : matGroups) {
            // Get the shader associated with the material
            auto material = MaterialTable::GetMaterial(matID);
            auto shader = material->GetShader(); // Assuming Material has a method to retrieve its shader

            shader->Use();  // Bind the shader
            shader->SetUniform("u_ViewProjMtx", cam.GetViewProjMatrix());
            shader->SetUniform("u_CamPos", cam.GetPosition());
            material->Apply(shader);    // Apply material properties
            MaterialTable::ApplyMaterialTextures(shader);   // Apply material textures

            for (const auto& [entity, worldMtx] : entityPairs) {
                auto const& mesh = entity.GetComponent<Component::Mesh>();
                Graphics::Renderer::SubmitInstance(mesh.mesh, worldMtx, Color::COLOR_WHITE, entity.GetEntityID(), matID);
            }
            mSpec.pipeline->GetSpec().instanceLayout;
            // Flush all collected instances and render them in a single draw call
            Renderer::RenderInstances();
        }

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