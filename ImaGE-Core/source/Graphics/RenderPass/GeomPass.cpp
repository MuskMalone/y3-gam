#include <pch.h>
#include "GeomPass.h"
#include "Core/Entity.h"
#include <Graphics/EditorCamera.h>
#include <Core/Components/Components.h>
#include "Color.h"
#include "Asset/IGEAssets.h"
#include <Graphics/Renderer.h>
#include <Graphics/RenderPass/ShadowPass.h>
#include "Graphics/MaterialData.h"

#include "Graphics/RenderAPI.h"

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

      Renderer::RenderSceneBegin(cam.viewProjMatrix);

      // Use a map to group entities by material ID
      MatGroupsMap matGroups;

      //==========================LIGHTS==========================================================
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
      //==========================LIGHTS END===================================================
      auto& ecsMan{ ECS::EntityManager::GetInstance() };

      //auto const& entitiesMat = ecsMan.GetAllEntitiesWithComponents< Component::Transform, Component::Mesh>();
      //for (auto const& e : entitiesMat) {
      //    ECS::Entity entity{ e };     
      //    auto const& mesh = entity.GetComponent<Component::Mesh>();auto const& mesh = entity.GetComponent<Component::Mesh>();
      //    // Skip if mesh is null
      //    if (!mesh.meshSource.IsValid()) { 
      //        continue;
      //    }
      //    int matIdx = 0;
      //    if (entity.HasComponent<Component::Material>()) {
      //        auto const& material = entity.GetComponent<Component::Material>();
      //        matIdx = material.matIdx;
      //    }

      //    auto const& xform = entity.GetComponent<Component::Transform>();

      //    Renderer::SubmitInstance(mesh.meshSource, xform.worldMtx, Color::COLOR_WHITE, entity.GetEntityID(), matIdx );

      //=================================================SUBMESH VERSION===============================================================
          // Group entities by material ID and gather instances for submeshes
      for (ECS::Entity const& entity : entities) {
          if (!entity.HasComponent<Component::Mesh>()) continue;

          auto const& xform = entity.GetComponent<Component::Transform>();
          auto const& mesh = entity.GetComponent<Component::Mesh>();

          if (!mesh.meshSource.IsValid()) continue;

          uint32_t matID = 0;
          if (entity.HasComponent<Component::Material>()) {
              auto const& matComponent = entity.GetComponent<Component::Material>();
              matID = MaterialTable::GetMaterialIndexByGUID(matComponent.materialGUID);//matComponent.matIdx;
          }

          // Group entity pairs by material ID for rendering
          matGroups[matID].emplace_back(entity, xform.worldMtx);
      }

      // Render each material group
      for (const auto& [matID, entityPairs] : matGroups) {
          // Get the shader associated with the material
          auto material = MaterialTable::GetMaterial(matID);
          auto shader = material->GetShader();

          shader->Use();
          shader->SetUniform("u_ViewProjMtx", cam.viewProjMatrix);
          shader->SetUniform("u_CamPos", cam.position);

          // Light Info
          shader->SetUniform("numlights", numlights);
          shader->SetUniform("u_type", u_type, maxLights);
          shader->SetUniform("u_LightDirection", u_LightDirection, maxLights);
          shader->SetUniform("u_LightColor", u_LightColor, maxLights);
          shader->SetUniform("u_LightPos", u_LightPos, maxLights);
          shader->SetUniform("u_InnerSpotAngle", u_InnerSpotAngle, maxLights);
          shader->SetUniform("u_OuterSpotAngle", u_OuterSpotAngle, maxLights);
          shader->SetUniform("u_LightIntensity", u_LightIntensity, maxLights);
          shader->SetUniform("u_Range", u_Range, maxLights);

          {
            // Set shadow uniforms
            auto const& shadowPass = Renderer::GetPass<ShadowPass>();
            shader->SetUniform("u_ShadowsActive", shadowPass->IsActive());
            if (shadowPass->IsActive()) {
              shader->SetUniform("u_LightSpaceMtx", shadowPass->GetLightSpaceMatrix());
              shader->SetUniform("u_ShadowMap", static_cast<int>(shadowPass->BindShadowMap()));
              shader->SetUniform("u_ShadowBias", shadowPass->GetShadowBias());
              shader->SetUniform("u_ShadowSoftness", shadowPass->GetShadowSoftness());
            }
          }

          material->Apply(shader);
          MaterialTable::ApplyMaterialTextures(shader);

          // Submit all instances of each submesh for this material
          for (const auto& [entity, worldMtx] : entityPairs) {
            auto const& mesh = entity.GetComponent<Component::Mesh>();

            //Graphics::Renderer::SubmitSubmeshInstance(mesh.meshSource, 0, worldMtx, Color::COLOR_WHITE, entity.GetEntityID(), matID);
            Graphics::Renderer::SubmitInstance(mesh.meshSource, worldMtx, Color::COLOR_WHITE, entity.GetEntityID(), matID, mesh.submeshIdx);
          }


          //mSpec.pipeline->GetSpec().instanceLayout;
          Renderer::RenderSubmeshInstances();  // Render all instances for the material group
          Texture::ResetTextureUnits(); // Unbind textures after each group
      }

      if (cam.isEditor) {
          auto const& lights = ecsMan.GetAllEntitiesWithComponents<Component::Light, Component::Transform>();
          for (auto const& light : lights) {
              auto const& xform = ECS::Entity{ light }.GetComponent<Component::Transform>();
              auto const& lightComp = ECS::Entity{ light }.GetComponent<Component::Light>();
              Renderer::DrawLightGizmo(lightComp, xform, cam, ECS::Entity{light}.GetEntityID());
          }
          //auto const& cameras = ecsMan.GetAllEntitiesWithComponents<Component::Camera>();
          //for (auto const& camera : cameras) {
          //    if (!ECS::Entity{ camera }.IsActive()) continue;
          //    auto const& camComp = ECS::Entity{ camera }.GetComponent<Component::Camera>();
          //    auto const& xform = ECS::Entity{ camera }.GetComponent<Component::Transform>();
          //    Renderer::DrawSprite(xform.worldPos, glm::vec2{ xform.worldScale }, xform.worldRot, IGE_ASSETMGR.GetAsset<IGE::Assets::TextureAsset>(Renderer::mIcons[2])->mTexture, Color::COLOR_WHITE, ECS::Entity { camera }.GetEntityID(), true, cam);
          //}

      }

      for (ECS::Entity const& entity : entities) {
        if (entity.HasComponent<Component::Sprite2D>()) {
          auto const& sprite = entity.GetComponent<Component::Sprite2D>();
          auto const& xform = entity.GetComponent<Component::Transform>();
          if (sprite.textureAsset)
              Renderer::DrawSprite(xform.worldPos, xform.worldScale, xform.worldRot, IGE_ASSETMGR.GetAsset<IGE::Assets::TextureAsset>(sprite.textureAsset)->mTexture, sprite.color);
          else
              Renderer::DrawQuad(xform.worldPos, glm::vec2{ xform.worldScale }, xform.worldRot, sprite.color);

        }
      }

      Renderer::RenderSceneEnd();
      //=================================================SUBMESH VERSION END===========================================================

      End();

      auto const& fb = mSpec.pipeline->GetSpec().targetFramebuffer;

      // Check if mOutputTexture is null or if dimensions don’t match
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
