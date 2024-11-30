#include <pch.h>
#include "GeomPass.h"
#include "Core/Entity.h"
#include <Graphics/Camera/EditorCamera.h>
#include <Core/Components/Components.h>
#include "Color.h"
#include "Asset/IGEAssets.h"
#include <Graphics/Renderer.h>
#include <Graphics/RenderPass/ShadowPass.h>
#include <Graphics/Renderpass/PostProcessPass.h>
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
      GetTargetFramebuffer()->ClearAttachmentInt(1, -1);

      //=================================================SUBMESH VERSION===============================================================
      // STEP UNO: Collect entities into shader groups!!!
      ShaderGroupMap const shaderGroups{ GroupEntities(entities) };

      // get light data to pass into shader
      LightUniforms<sMaxLights> const lightUniforms{ GetLightData<sMaxLights>(entities) };

      unsigned matCount{ static_cast<unsigned>(MaterialTable::GetMaterialCount()) };
      // STEP TRES: Render shader groups
      for (auto const&[shader, matGrp] : shaderGroups) {

        // Only bind the shader if it's different from the currently bound one
        shader->Use();

        bool isUnlitShader = (shader == ShaderLibrary::Get("Unlit"));
        shader->SetUniform("u_ViewProjMtx", cam.viewProjMatrix);
        shader->SetUniform("u_ViewMtx", cam.viewMatrix);
        if (!isUnlitShader) {
          shader->SetUniform("u_CamPos", cam.position);

          // Light Info
          lightUniforms.SetUniforms(shader);

          // Set shadow uniforms
          auto const& shadowPass = Renderer::GetPass<ShadowPass>();
          shader->SetUniform("u_ShadowsActive", shadowPass->IsActive());
          if (shadowPass->IsActive()) {
            shader->SetUniform("u_LightSpaceMtx", shadowPass->GetLightSpaceMatrix());
            shadowPass->BindShadowMap(Texture::sShadowMapTexUnit);
            shader->SetUniform("u_ShadowMap", static_cast<int>(Texture::sShadowMapTexUnit));
            shader->SetUniform("u_ShadowBias", shadowPass->GetShadowBias());
            shader->SetUniform("u_ShadowSoftness", shadowPass->GetShadowSoftness());
          }
        }

        for (auto const& [matGrp, entityData] : matGrp) {
          // set the offset to subtract from the index
          unsigned const batchStart{ static_cast<unsigned>(matGrp) * MaterialTable::sMaterialsPerBatch },
            batchEnd{ matCount - batchStart >= MaterialTable::sMaterialsPerBatch
            ? batchStart + MaterialTable::sMaterialsPerBatch - 1 : matCount - 1 };

          MaterialTable::ApplyMaterialTextures(shader, batchStart, batchEnd);

          int offset = matGrp == 0 ? 0 : -static_cast<int>(batchStart);
          shader->SetUniform("u_MatIdxOffset", offset);
          // Render all instances for this material
          for (const auto&[worldMtx, entity, matIdx] : entityData) {
            auto const& mesh = entity.GetComponent<Component::Mesh>();
            Graphics::Renderer::SubmitInstance(mesh.meshSource, worldMtx, Color::COLOR_WHITE,
              entity.GetEntityID(), matIdx, mesh.submeshIdx);
          }

          Renderer::RenderSubmeshInstances();  // Flush all instances for this material group
        }
      }

//========================================2D Sprite Rendering=========================================================================================
      Renderer::RenderSceneBegin(cam.viewProjMatrix, cam);
      std::vector<ECS::Entity> opaqueSprites;
      std::vector<ECS::Entity> transparentSprites;

      // Group entities into opaque and transparent sprites
      for (ECS::Entity const& entity : entities) {
          if (!entity.HasComponent<Component::Sprite2D>()) continue;

          auto const& sprite = entity.GetComponent<Component::Sprite2D>();
          if (sprite.isTransparent) {
              transparentSprites.push_back(entity);
          }
          else {
              opaqueSprites.push_back(entity);
          }
      }

      // Sort transparent sprites by distance to the camera (back-to-front)
      std::sort(transparentSprites.begin(), transparentSprites.end(),
          [&cam](auto const& a, auto const& b) {
              auto const& aTransform = a.GetComponent<Component::Transform>();
              auto const& bTransform = b.GetComponent<Component::Transform>();

              // Compute distance from the camera to each sprite
              float distanceA = glm::length2(cam.position - aTransform.worldPos); // Squared distance
              float distanceB = glm::length2(cam.position - bTransform.worldPos); // Squared distance

              return distanceA > distanceB; // Sort back-to-front
          });

      // Render opaque sprites
      for (ECS::Entity const& entity : opaqueSprites) {
          auto const& sprite = entity.GetComponent<Component::Sprite2D>();
          auto const& xform = entity.GetComponent<Component::Transform>();

          if (sprite.textureAsset) {
              Renderer::DrawSprite(xform.worldPos, xform.worldScale, xform.worldRot,
                  IGE_ASSETMGR.GetAsset<IGE::Assets::TextureAsset>(sprite.textureAsset)->mTexture,
                  sprite.color, entity.GetEntityID());
          }
          else {
              Renderer::DrawQuad(xform.worldPos, glm::vec2{ xform.worldScale }, xform.worldRot, sprite.color, entity.GetEntityID());
          }
      }

      Renderer::FlushBatch(); // Flush opaque sprites

      // Render transparent sprites
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glDepthMask(GL_FALSE); // Disable depth writing

      for (ECS::Entity const& entity : transparentSprites) {
          auto const& sprite = entity.GetComponent<Component::Sprite2D>();
          auto const& xform = entity.GetComponent<Component::Transform>();

          if (sprite.textureAsset) {
              Renderer::DrawSprite(xform.worldPos, xform.worldScale, xform.worldRot,
                  IGE_ASSETMGR.GetAsset<IGE::Assets::TextureAsset>(sprite.textureAsset)->mTexture,
                  sprite.color, entity.GetEntityID());
          }
          else {
              Renderer::DrawQuad(xform.worldPos, glm::vec2{ xform.worldScale }, xform.worldRot, sprite.color, entity.GetEntityID());
          }
      }

      Renderer::FlushBatch(); // Flush transparent sprites
      glDepthMask(GL_TRUE); // Re-enable depth writing

      //Renderer::RenderSceneEnd();
      //=================================================SUBMESH VERSION END===========================================================
      End();

      auto const& fb = mSpec.pipeline->GetSpec().targetFramebuffer;

      if (!cam.isEditor)
          mPickFramebuffer = fb; // for game view only

      // Check if mOutputTexture is null or if dimensions don’t match
      if (!mOutputTexture || mOutputTexture->GetWidth() != fb->GetFramebufferSpec().width || mOutputTexture->GetHeight() != fb->GetFramebufferSpec().height) {
          // Create or resize mOutputTexture based on the framebuffer's specs
          mOutputTexture = std::make_shared<Graphics::Texture>(fb->GetFramebufferSpec().width, fb->GetFramebufferSpec().height);
          mDepthTexture = std::make_shared<Graphics::Texture>(fb->GetFramebufferSpec().width, fb->GetFramebufferSpec().height, GL_DEPTH24_STENCIL8);
          mRedTexture = std::make_shared<Graphics::Texture>(fb->GetFramebufferSpec().width, fb->GetFramebufferSpec().height, GL_R32I);
      }

      // Perform the copy operation
      if (mOutputTexture) {
          mOutputTexture->CopyFrom(fb->GetColorAttachmentID(), fb->GetFramebufferSpec().width, fb->GetFramebufferSpec().height);
          mDepthTexture->CopyFrom(fb->GetDepthAttachmentID(), fb->GetFramebufferSpec().width, fb->GetFramebufferSpec().height);
          mRedTexture->CopyFrom(fb->GetColorAttachmentID(1), fb->GetFramebufferSpec().width, fb->GetFramebufferSpec().height);
      }

      auto const& pass{ Renderer::GetPass<PostProcessingPass>() };
      auto& positionTexture{ pass->mPositionGBuffer };
      if (!positionTexture || positionTexture->GetWidth() != fb->GetFramebufferSpec().width || positionTexture->GetHeight() != fb->GetFramebufferSpec().height) {
          // Create or resize mOutputTexture based on the framebuffer's specs
          positionTexture = std::make_shared<Graphics::Texture>(fb->GetFramebufferSpec().width, fb->GetFramebufferSpec().height, false, GL_RGBA32F);
      }

      // Perform the copy operation
      if (positionTexture) {
          positionTexture->CopyFrom(fb->GetColorAttachmentID(2), fb->GetFramebufferSpec().width, fb->GetFramebufferSpec().height);
      }
  }

  std::shared_ptr<Texture> GeomPass::GetDepthTexture() {
      return mDepthTexture;
  }

  std::shared_ptr<Texture> GeomPass::GetEntityTexture(){
      return mRedTexture;
  }

  std::shared_ptr<Framebuffer> GeomPass::GetGameViewFramebuffer() const {
      return mPickFramebuffer;
  }

  GeomPass::ShaderGroupMap GeomPass::GroupEntities(std::vector<ECS::Entity> const& entities) {
    ShaderGroupMap shaderToMatGrp; // group by shader, then by material batch

    auto CalculateMatBatch = [](int matID) -> int {
        return (matID > 0) ? (matID - 1) / MaterialTable::sMaterialsPerBatch : 0;
    };

    for (ECS::Entity const& entity : entities) {
      if (!entity.HasComponent<Component::Mesh>() || 
        !entity.GetComponent<Component::Mesh>().meshSource.IsValid()) { continue; }

      int matID = 0;
      if (entity.HasComponent<Component::Material>()) {
        auto const& matComp = entity.GetComponent<Component::Material>();
        matID = matComp.matIdx;
      }

      // use corresponding shader to index mat grp
      MatGroupMap& matGroups{ shaderToMatGrp[MaterialTable::GetMaterial(matID)->GetShader()] };
      auto const& xform = entity.GetComponent<Component::Transform>();
      
      // add entity based on material batch number
      int const matBatch{ CalculateMatBatch(matID) };
      matGroups[matBatch].emplace_back(xform.worldMtx, entity, matID);
    }

    return shaderToMatGrp;
  }

} // namespace Graphics

namespace {
  template <unsigned N>
  LightUniforms<N> GetLightData(std::vector<ECS::Entity> const& entities) {
    LightUniforms<N> lightUniforms{};
    int numLights{};

    //Get the list of light
    std::vector<ECS::Entity> lights{};
    std::cout << "ALL LIGHT-----------------------------\n";
    for (ECS::Entity const& entity : entities) {
      if (!entity.HasComponent<Component::Light>()) { continue; }
      std::cout << entity.GetTag() << "\n";
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
    std::cout << "--------------------------------------\n";
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
