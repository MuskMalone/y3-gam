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

namespace {
  template <unsigned N>
  struct LightUniforms {
    glm::vec3 u_LightDirection[N]; // Directional light direction in world space
    glm::vec3 u_LightColor[N];     // Directional light color
    int u_type[N];       // Camera position in world space

    // For spotlight
    glm::vec3 u_LightPos[N]; // Position of the spotlight
    float u_InnerSpotAngle[N]; // Inner spot angle in degrees
    float u_OuterSpotAngle[N]; // Outer spot angle in degrees
    float u_LightIntensity[N]; // Intensity of the light
    float u_Range[N]; // Maximum range of the spotlight

    int numLights;

    void SetUniforms(std::shared_ptr<Graphics::Shader> const& shader) const;
  };

  template <unsigned N>
  LightUniforms<N> GetLightData(std::vector<ECS::Entity> const& entities);
}

namespace Graphics {
  GeomPass::GeomPass(const RenderPassSpec& spec) : RenderPass(spec) {}

  void GeomPass::Render(CameraSpec const& cam, std::vector<ECS::Entity> const& entities) {
      Begin();
     // Renderer::Clear();
      //auto shader = mSpec.pipeline->GetShader();

      Renderer::RenderSceneBegin(cam.viewProjMatrix);

      //=================================================SUBMESH VERSION===============================================================
      //MatGroupsMap matGroups; // Use a map to group entities by material ID

      // STEP UNO: Collect entities into material groups!!!
      //    *i moved it into a function
      std::vector<MaterialGroup> materialGroups{ CreateMaterialGroups(entities) };
      
      // STEP DOS: Prepare material groups for sorting based on shader used!!!
      std::sort(materialGroups.begin(), materialGroups.end(),
          [](const MaterialGroup& a, const MaterialGroup& b) {
              return a.shader.get() < b.shader.get(); // Sort by shader pointer for grouping
          });

      // get light data to pass into shader
      //    *moved light stuff into a function too
      LightUniforms<sMaxLights> const lightUniforms{ GetLightData<sMaxLights>(entities) };

      // STEP TRES POINT UNO: Render material groups
      
      // STEP TRES POINT DOS: Render in batches based on material count
      size_t const matCount{ MaterialTable::GetMaterialCount() };
      // perform a render call for each batch of materials based on sMaterialsPerBatch
      // until all materials are accounted for
      for (size_t matIdx{}; matIdx < matCount; matIdx += MaterialTable::sMaterialsPerBatch) {
        std::shared_ptr<Shader> currShader = nullptr;
        for (const auto& group : materialGroups) {
          auto shader = group.shader;

          // Only bind the shader if it's different from the currently bound one
          if (shader != currShader) {
            shader->Use();
            currShader = shader;

            bool isUnlitShader = (shader == ShaderLibrary::Get("Unlit"));
            shader->SetUniform("u_ViewProjMtx", cam.viewProjMatrix);

            if (!isUnlitShader) {
              shader->SetUniform("u_CamPos", cam.position);

              // Light Info
              lightUniforms.SetUniforms(shader);

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
          }

          // Apply material-specific properties
          auto const& material = MaterialTable::GetMaterial(group.matID);
          material->Apply(shader);

          // clamp the end of batch to matCount - 1
          size_t const endBatch{ matCount - matIdx >= MaterialTable::sMaterialsPerBatch ? matIdx + MaterialTable::sMaterialsPerBatch - 1 : matCount - 1 };

          MaterialTable::ApplyMaterialTextures(shader, matIdx, endBatch);

          // Render all instances for this material
          for (const auto& [entity, worldMtx] : group.entityPairs) {
            auto const& mesh = entity.GetComponent<Component::Mesh>();
            Graphics::Renderer::SubmitInstance(mesh.meshSource, worldMtx, Color::COLOR_WHITE, entity.GetEntityID(), (group.matID % (MaterialTable::sMaterialsPerBatch + 1)), mesh.submeshIdx);
          }

          //Renderer::RenderSubmeshInstances();  // Flush all instances for this material group
          //Texture::ResetTextureUnits();
        }
        Renderer::RenderSubmeshInstances();
        Texture::ResetTextureUnits();
      
      }

      if (cam.isEditor) {
          auto const& lights{ IGE_ENTITYMGR.GetAllEntitiesWithComponents<Component::Light, Component::Transform>() };
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
              Renderer::DrawSprite(xform.worldPos, xform.worldScale, xform.worldRot, IGE_ASSETMGR.GetAsset<IGE::Assets::TextureAsset>(sprite.textureAsset)->mTexture, sprite.color, entity.GetEntityID());
          else
              Renderer::DrawQuad(xform.worldPos, glm::vec2{ xform.worldScale }, xform.worldRot, sprite.color, entity.GetEntityID());

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

  std::vector<GeomPass::MaterialGroup> GeomPass::CreateMaterialGroups(std::vector<ECS::Entity> const& entities) {
    std::vector<MaterialGroup> materialGroups;
    std::unordered_map<int, unsigned> existingGroups; // matID, index

    for (ECS::Entity const& entity : entities) {
      if (!entity.HasComponent<Component::Mesh>()) { continue; }

      auto const& mesh = entity.GetComponent<Component::Mesh>();

      if (!mesh.meshSource.IsValid()) continue;

      int matID = 0;
      if (entity.HasComponent<Component::Material>()) {
        auto const& matComp = entity.GetComponent<Component::Material>();
        matID = matComp.matIdx;
      }

      auto const& xform = entity.GetComponent<Component::Transform>();

      // Find or create the MaterialGroup
      if (!existingGroups.contains(matID)) {
        // Create an entry in the map
        existingGroups.emplace(matID, materialGroups.size());

        // Add the new MaterialGroup
        auto material = MaterialTable::GetMaterial(matID);
        materialGroups.push_back({ matID, material->GetShader(), {{entity, xform.worldMtx}} });
      }
      else {
        // Add to existing group
        materialGroups[existingGroups[matID]].entityPairs.emplace_back(EntityXform{ entity, xform.worldMtx });
      }
    }

    return materialGroups;
  }

} // namespace Graphics

namespace {
  template <unsigned N>
  LightUniforms<N> GetLightData(std::vector<ECS::Entity> const& entities) {
    LightUniforms<N> lightUniforms{};
    int numLights{};

    //Get the list of light
    std::vector<ECS::Entity> lights{};
    for (ECS::Entity const& entity : entities) {
      if (!entity.HasComponent<Component::Light>()) { continue; }

      auto const& light = entity.GetComponent<Component::Light>();
      lightUniforms.u_type[numLights] = light.type;
      lightUniforms.u_LightDirection[numLights] = entity.GetComponent<Component::Transform>().worldRot * light.forwardVec; // Directional light direction in world space
      lightUniforms.u_LightColor[numLights] = light.color;     // Directional light color

      //For spotlight
      lightUniforms.u_LightPos[numLights] = entity.GetComponent<Component::Transform>().worldPos; // Position of the spotlight
      lightUniforms.u_InnerSpotAngle[numLights] = light.mInnerSpotAngle; // Inner spot angle in degrees
      lightUniforms.u_OuterSpotAngle[numLights] = light.mOuterSpotAngle; // Outer spot angle in degrees
      lightUniforms.u_LightIntensity[numLights] = light.mLightIntensity; // Intensity of the light
      lightUniforms.u_Range[numLights] = light.mRange; // Maximum range of the spotlight
      ++numLights;
    }
    lightUniforms.numLights = numLights;

    return lightUniforms;
  }

  template <unsigned N>
  void LightUniforms<N>::SetUniforms(std::shared_ptr<Graphics::Shader> const& shader) const {
    shader->SetUniform("numlights", numLights);
    shader->SetUniform("u_type", u_type, N);
    shader->SetUniform("u_LightDirection", u_LightDirection, N);
    shader->SetUniform("u_LightColor", u_LightColor, N);
    shader->SetUniform("u_LightPos", u_LightPos, N);
    shader->SetUniform("u_InnerSpotAngle", u_InnerSpotAngle, N);
    shader->SetUniform("u_OuterSpotAngle", u_OuterSpotAngle, N);
    shader->SetUniform("u_LightIntensity", u_LightIntensity, N);
    shader->SetUniform("u_Range", u_Range, N);
  }
}
