#include <pch.h>
#include "ShadowPass.h"
#include "Core/Entity.h"
#include <Graphics/EditorCamera.h>
#include <Graphics/RenderAPI.h>
#include <Core/Components/Light.h>
#include <Core/Components/Transform.h>
#include <Core/Components/Mesh.h>

namespace Graphics {
  ShadowPass::ShadowPass(const RenderPassSpec& spec) : RenderPass(spec), mLightSpaceMtx{}, mShadowSoftness{}, mShadowBias{}, mActive{ false } {
    
  }

  void ShadowPass::Render(EditorCamera const& cam, std::vector<ECS::Entity> const& entities) {
    mActive = LocateLightSource(cam, entities);
    if (!mActive) { return; }

    StartRender();

    auto const& shader = mSpec.pipeline->GetShader();
    // render the scene normally
    for (ECS::Entity const& entity : entities) {
      if (!entity.HasComponent<Component::Mesh>() || entity.HasComponent<Component::Light>()) { continue; }
      Component::Mesh const& mesh{ entity.GetComponent<Component::Mesh>() };
      // skip if no mesh or if it doesn't cast shadows
      if (!mesh.meshSource.IsValid() || !mesh.castShadows) { continue; }

      Graphics::Renderer::SubmitInstance(
        entity.GetComponent<Component::Mesh>().meshSource,
        entity.GetComponent<Component::Transform>().worldMtx,
        Color::COLOR_WHITE,
        entity.GetEntityID(),
        0
      );
    }
    Renderer::RenderInstances();

    EndRender();
  }

  bool ShadowPass::LocateLightSource(EditorCamera const& cam, std::vector<ECS::Entity> const& entities) {
    bool found{ false };

    // iterate through entities to find the shadow-casting light
    for (ECS::Entity const& entity : entities) {
      if (!entity.HasComponent<Component::Light>()) { continue; }

      Component::Light const& light{ entity.GetComponent<Component::Light>() };
      Component::Transform const& transform{ entity.GetComponent<Component::Transform>() };

      // only care about shadow casters
      if (!light.castShadows || light.type != Component::LightType::DIRECTIONAL) {
        continue;
      }
      found = true;

      mShadowBias = light.bias;
      mShadowSoftness = light.softness;
      SetLightUniforms(cam, transform.worldRot * light.forwardVec, light.nearPlaneMultiplier, transform.worldPos);

      break;
    }

    return found;
  }

  void ShadowPass::StartRender() {
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);
    Begin();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void ShadowPass::EndRender() {
    End();
    glCullFace(GL_BACK);
  }

  void ShadowPass::SetLightUniforms(EditorCamera const& cam, glm::vec3 const& lightDir, float nearPlaneMultiplier, glm::vec3 test) {
    // NDC frustrum corners
    std::array<glm::vec4, 8> frustrumCorners = {
      glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), // Near-btm-left
      glm::vec4(1.0f, -1.0f, -1.0f, 1.0f),  // Near-btm-right
      glm::vec4(1.0f, 1.0f, -1.0f, 1.0f),   // Near-top-right
      glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f),  // Near-top-left
      glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f),  // Far-btm-left
      glm::vec4(1.0f, -1.0f, 1.0f, 1.0f),   // Far-btm-right
      glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),    // Far-top-right
      glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f)    // Far-top-left
    };

    // transform NDC frustrum corners into world space
    glm::mat4 const invViewProj{ glm::inverse(cam.GetViewProjMatrix()) };
    glm::vec3 center{};
    for (glm::vec4& corner : frustrumCorners) {
      corner = invViewProj * corner;
      corner /= corner.w;

      center += glm::vec3(corner); // accumulate to compute the center at the same time
    }
    center /= 8.f;

    // get light view mtx
    glm::mat4 const lightView{ glm::lookAt(center - lightDir, center, glm::vec3(0.f, 1.f, 0.f))};

    // find the min and max extents of the frustrum in light space
    glm::vec3 min{ FLT_MAX }, max{ -FLT_MAX };
    for (glm::vec4 const& corner : frustrumCorners) {
      glm::vec3 const lightSpaceCorner{ lightView * corner };
      min = glm::min(min, lightSpaceCorner);
      max = glm::max(max, lightSpaceCorner);
    }

    // pull back near plane and push back far plane based on multiplier
    //float const nearPlane{ min.z < 0.f ? min.z * nearPlaneMultiplier : min.z / nearPlaneMultiplier },
    //  farPlane{ max.z < 0.f ? max.z / nearPlaneMultiplier : max.z * nearPlaneMultiplier };
    min *= nearPlaneMultiplier;
    max *= nearPlaneMultiplier;

    auto const& shader = mSpec.pipeline->GetShader();
    shader->Use();

    shader->SetUniform("u_Near", min.z);
    shader->SetUniform("u_Far", max.z);

    mLightSpaceMtx = glm::ortho(min.x, max.x,
      min.y, max.y, min.z, max.z) * lightView;

    shader->SetUniform("u_LightSpaceMtx", mLightSpaceMtx);
  }

  uint32_t ShadowPass::BindShadowMap() {
    return Texture::BindToNextAvailUnit(GetShadowMapBuffer());
  }

} // namespace Graphics
