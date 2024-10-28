#include <pch.h>
#include "ShadowPass.h"
#include "Core/Entity.h"
#include <Graphics/EditorCamera.h>
#include <Graphics/RenderAPI.h>
#include <Core/Components/Light.h>
#include <Core/Components/Transform.h>
#include <Core/Components/Mesh.h>

#define CAMERA_VIEW

namespace {
  static constexpr std::array<glm::vec4, 8> sFrustrumCorners = {
    glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), // Near-bottom-left
    glm::vec4(1.0f, -1.0f, -1.0f, 1.0f),  // Near-bottom-right
    glm::vec4(1.0f, 1.0f, -1.0f, 1.0f),   // Near-top-right
    glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f),  // Near-top-left
    glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f),  // Far-bottom-left
    glm::vec4(1.0f, -1.0f, 1.0f, 1.0f),   // Far-bottom-right
    glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),    // Far-top-right
    glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f)    // Far-top-left
  };
}

namespace Graphics {

  ShadowPass::ShadowPass(const RenderPassSpec& spec) : RenderPass(spec), mActive{ false } {

  }

  void ShadowPass::Render(EditorCamera const& cam) {
    StartRender();
    mActive = SetLightUniforms(cam);

    if (!mActive) { EndRender(); return; }

    auto const& shader = mSpec.pipeline->GetShader();
    // render the scene normally
    for (ECS::Entity entity : ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::Transform, Component::Mesh>()) {
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

  bool ShadowPass::SetLightUniforms(EditorCamera const& cam) {
    bool found{ false };

    // iterate through entities to find the shadow-casting light
    for (ECS::Entity entity : ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::Light>()) {
      Component::Light const& light{ entity.GetComponent<Component::Light>() };
      Component::Transform const& transform{ entity.GetComponent<Component::Transform>() };

      // only care about shadow casters
      if (!light.castShadows) {
        continue;
      }
      found = true;

      // set uniforms
      auto const& shader = mSpec.pipeline->GetShader();
      shader->Use();

      auto const orthoPlanes{ GetLightProjPlanes(cam, transform.worldPos, light.direction) };

#ifdef CAMERA_VIEW
      shader->SetUniform("near", cam.GetNearPlane());
      shader->SetUniform("far", cam.GetFarPlane());
#else
      shader->SetUniform("near", orthoPlanes.first.z);
      shader->SetUniform("far", orthoPlanes.second.z);
#endif

      shader->SetUniform("u_LightProjMtx", glm::ortho(orthoPlanes.first.x, orthoPlanes.second.x,
        orthoPlanes.first.y, orthoPlanes.second.y, orthoPlanes.first.z, orthoPlanes.second.z));
      shader->SetUniform("u_ViewProjMtx", cam.GetViewProjMatrix());

      break;
    }

    return found;
  }

  void ShadowPass::StartRender() {
    glEnable(GL_DEPTH_TEST);
    Begin();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  void ShadowPass::EndRender() {
    End();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mSpec.pipeline->GetShader()->Unuse();
  }

  std::pair<glm::vec3, glm::vec3> ShadowPass::GetLightProjPlanes(EditorCamera const& cam, glm::vec3 const& lightPos, glm::vec3 const& lightDir) {
    glm::mat4 const invViewProj{ glm::inverse(cam.GetViewProjMatrix()) * glm::lookAt(lightPos, glm::vec3(0.f)/*lightPos + lightDir*/, glm::vec3(0.f, 1.f, 0.f))};
    auto frustrumCorners{ sFrustrumCorners };

    glm::vec4 min(FLT_MAX), max(-FLT_MAX);
    for (glm::vec4& corner : frustrumCorners) {
      corner = invViewProj * corner;
      corner /= corner.w;
      min = glm::min(min, corner);
      max = glm::max(max, corner);
    }

    return { min, max };
  }

} // namespace Graphics
