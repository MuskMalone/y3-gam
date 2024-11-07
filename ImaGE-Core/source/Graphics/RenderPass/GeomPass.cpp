#include <pch.h>
#include "GeomPass.h"
#include "Core/Entity.h"
#include <Graphics/EditorCamera.h>
#include <Core/Components/Components.h>
#include "Color.h"
#include "Asset/IGEAssets.h"
#include <Graphics/Renderer.h>
#include <Graphics/RenderPass/ShadowPass.h>
#include "Graphics/Material.h"

namespace Graphics {
  using EntityXform = std::pair<ECS::Entity, glm::mat4>;
  using MatGroup = std::vector<EntityXform>;
  using MatGroupsMap = std::unordered_map<uint32_t, MatGroup>;

  GeomPass::GeomPass(const RenderPassSpec& spec) : RenderPass(spec) {

  }

  void GeomPass::Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities) {
      Begin();
      Renderer::Clear();
      //auto shader = mSpec.pipeline->GetShader();

      // Use a map to group entities by material ID
      MatGroupsMap matGroups;

      const unsigned int maxLights = 10;
      int numlights{ 0 };

      int u_type[maxLights];       // Camera position in world space
      glm::vec3 u_LightDirection[maxLights]; // Directional light direction in world space
      glm::vec3 u_LightColor[maxLights];     // Directional light color

      // For spotlight
      glm::vec3 u_LightPos[maxLights]; // Position of the spotlight
      float u_InnerSpotAngle[maxLights]; // Inner spot angle in degrees
      float u_OuterSpotAngle[maxLights]; // Outer spot angle in degrees
      float u_LightIntensity[maxLights]; // Intensity of the light
      float u_Range[maxLights]; // Maximum range of the spotlight

      //Get the list of light
      std::vector<ECS::Entity> lights{};
      for (ECS::Entity const& entity : entities) {
          if (!entity.HasComponent<Component::Light>()) { continue; }

          auto const& light = entity.GetComponent<Component::Light>();
          u_type[numlights] = light.type;
          u_LightDirection[numlights] = entity.GetComponent<Component::Transform>().worldRot * light.forwardVec; // Directional light direction in world space
          u_LightColor[numlights] = light.color;     // Directional light color

          //For spotlight
          u_LightPos[numlights] = entity.GetComponent<Component::Transform>().worldPos; // Position of the spotlight
          u_InnerSpotAngle[numlights] = light.mInnerSpotAngle; // Inner spot angle in degrees
          u_OuterSpotAngle[numlights] = light.mOuterSpotAngle; // Outer spot angle in degrees
          u_LightIntensity[numlights] = light.mLightIntensity; // Intensity of the light
          u_Range[numlights] = light.mRange; // Maximum range of the spotlight
          ++numlights;
      }

      for (ECS::Entity const& entity : entities) {
          if (!entity.HasComponent<Component::Mesh>()) { continue; }

          auto const& xform = entity.GetComponent<Component::Transform>();
          auto const& mesh = entity.GetComponent<Component::Mesh>();

          // Skip if mesh is null
          if (!mesh.meshSource.IsValid()) {
              continue;
          }

          uint32_t matID = 0;
          if (entity.HasComponent<Component::MaterialComponent>()) {
              auto const& matComponent = entity.GetComponent<Component::MaterialComponent>();
              matID = matComponent.matIdx;
          }
          matGroups[matID].emplace_back(entity, xform.worldMtx);
      }

      // Now render each material group
      for (const auto& [matID, entityPairs] : matGroups) {
          // Get the shader associated with the material
          auto material = MaterialTable::GetMaterial(matID);
          auto shader = material->GetShader(); // Assuming Material has a method to retrieve its shader

          shader->Use();  // Bind the shader

          shader->SetUniform("u_ViewProjMtx", cam.viewProjMatrix);
          shader->SetUniform("u_CamPos", cam.position);

          //Light Info
          shader->SetUniform("numlights", numlights);
          shader->SetUniform("u_type", u_type, maxLights);
          shader->SetUniform("u_LightDirection", u_LightDirection, maxLights);
          shader->SetUniform("u_LightColor", u_LightColor, maxLights);

          shader->SetUniform("u_LightPos", u_LightPos, maxLights);
          shader->SetUniform("u_InnerSpotAngle", u_InnerSpotAngle, maxLights);
          shader->SetUniform("u_OuterSpotAngle", u_OuterSpotAngle, maxLights);
          shader->SetUniform("u_LightIntensity", u_LightIntensity, maxLights);
          shader->SetUniform("u_Range", u_Range, maxLights);

          // set shadow uniforms
          {
              auto const& shadowPass{ Renderer::GetPass<ShadowPass>() };
              shader->SetUniform("u_ShadowsActive", shadowPass->IsActive());
              if (shadowPass->IsActive()) {
                  shader->SetUniform("u_LightSpaceMtx", shadowPass->GetLightSpaceMatrix());
                  shader->SetUniform("u_ShadowMap", static_cast<int>(shadowPass->BindShadowMap()));
                  shader->SetUniform("u_ShadowBias", shadowPass->GetShadowBias());
                  shader->SetUniform("u_ShadowSoftness", shadowPass->GetShadowSoftness());
              }
          }

          material->Apply(shader);    // Apply material properties
          MaterialTable::ApplyMaterialTextures(shader);   // Apply material textures
          for (const auto& [entity, worldMtx] : entityPairs) {
              auto const& mesh = entity.GetComponent<Component::Mesh>();
              Graphics::Renderer::SubmitInstance(mesh.meshSource, worldMtx, Color::COLOR_WHITE, entity.GetEntityID(), matID);

          }
          mSpec.pipeline->GetSpec().instanceLayout;
          // Flush all collected instances and render them in a single draw call
          Renderer::RenderInstances();
          Texture::ResetTextureUnits(); // unbind texture units after each group
      }

      End();

      auto const& fb = mSpec.pipeline->GetSpec().targetFramebuffer;

      // Check if mOutputTexture is null or if dimensions don�t match
      if (!mOutputTexture || mOutputTexture->GetWidth() != fb->GetFramebufferSpec().width || mOutputTexture->GetHeight() != fb->GetFramebufferSpec().height) {
          // Create or resize mOutputTexture based on the framebuffer's specs
          mOutputTexture = std::make_shared<Graphics::Texture>(fb->GetFramebufferSpec().width, fb->GetFramebufferSpec().height);
      }

      // Perform the copy operation
      if (mOutputTexture) {
          mOutputTexture->CopyFrom(fb->GetColorAttachmentID(), fb->GetFramebufferSpec().width, fb->GetFramebufferSpec().height);
      }
  }

} // namespace Graphics
