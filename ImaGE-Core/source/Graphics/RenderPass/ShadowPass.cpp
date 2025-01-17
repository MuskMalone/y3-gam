#include <pch.h>
#include "ShadowPass.h"
#include "Core/Entity.h"
#include "Graphics/Renderer.h"
#include <Graphics/Camera/EditorCamera.h>
#include <Graphics/RenderAPI.h>
#include <Core/Components/Light.h>
#include <Core/Components/Transform.h>
#include <Core/Components/Mesh.h>
#include <Graphics/RenderSystem.h>
#include <Core/LayerManager/LayerManager.h>

namespace Graphics {
    ShadowPass::ShadowPass(const RenderPassSpec& spec) : RenderPass(spec),
      mLightSpaceMtx{}, mLightEntity{}, mShadowSoftness {}, mShadowBias{}, mActive{ false } {}

    void ShadowPass::Render(CameraSpec const& cam, [[maybe_unused]] std::vector<ECS::Entity> const& entities) {
      // only do shadow pass for game view
      if (cam.isEditor) {
        return;
      }

      // if the entity gets invalidated, or if the shadow settings have been modified, recompute the light mtx
      if (!mLightEntity || !mLightEntity.HasComponent<Component::Light>() || mLightEntity.GetComponent<Component::Light>().shadowConfig.shadowModified) {
        mActive = LocateLightEntity();
        if (!mActive) { return; }

        Init();
      }

      StartRender();
      SetLightUniforms();

      Layers::LayerManager& layerMan{ IGE_LAYERMGR };
      // iterate all entities in the ECS since this is from the light's perspective
      for (ECS::Entity entity : IGE_ENTITYMGR.GetAllEntitiesWithComponents<Component::Mesh>()) {
        // skip lights and layers that arent visible
        if (entity.HasComponent<Component::Light>()
          || !layerMan.IsLayerVisible(entity.GetComponent<Component::Layer>().name)) { continue; }

        Component::Mesh const& mesh{ entity.GetComponent<Component::Mesh>() };
        // skip if no mesh or if it doesn't cast shadows
        if (!mesh.meshSource.IsValid() || !mesh.castShadows) { continue; }

            Graphics::Renderer::SubmitInstance(
                entity.GetComponent<Component::Mesh>().meshSource,
                entity.GetComponent<Component::Transform>().worldMtx,
                Color::COLOR_WHITE,
                entity.GetEntityID()
            );
        }
        Renderer::RenderSubmeshInstances();

      EndRender();
    }

    void ShadowPass::Init() {
      Component::Transform const& transform{ mLightEntity.GetComponent<Component::Transform>() };
      Component::Light& light{ mLightEntity.GetComponent<Component::Light>() };

      mShadowBias = light.shadowConfig.bias;
      mShadowSoftness = light.shadowConfig.softness;

      ComputeLightSpaceMatrix(light, transform.worldRot);
    }

    bool ShadowPass::LocateLightEntity() {
      for (ECS::Entity const& entity : ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::Light>()) {
        if (!entity.IsActive()) { continue; }

        Component::Light const& light{ entity.GetComponent<Component::Light>() };
        // only care about static shadow casters
        if (!light.castShadows || light.type != Component::LightType::DIRECTIONAL || !light.shadowConfig.isStatic) {
          continue;
        }

        mLightEntity = entity;
        return true;
      }

      mLightEntity = {};
      return false;
    }

    void ShadowPass::ComputeLightSpaceMatrix(Component::Light& light, glm::quat const& lightRot) {
      glm::vec3 const center{ light.shadowConfig.customCenter ? light.shadowConfig.centerPos : glm::vec3() };
      mLightSpaceMtx = glm::lookAt(center - (lightRot * light.forwardVec), center, glm::vec3(0.f, 1.f, 0.f));

      // combine orthographic and view mtx to get light space mtx
      float const& bounds{ light.shadowConfig.scenesBounds * sDefaultSceneBounds };
      mLightSpaceMtx = glm::ortho(-bounds, bounds,
        -bounds, bounds, light.shadowConfig.nearPlane, light.shadowConfig.farPlane) * mLightSpaceMtx;
      light.shadowConfig.shadowModified = false;
    }

    void ShadowPass::StartRender() {
        Begin();
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_FRONT);
        Renderer::Clear();
    }

    void ShadowPass::EndRender() {
        End();
        glCullFace(GL_BACK);
    }

    void ShadowPass::SetLightUniforms(){
        auto const& shader = mSpec.pipeline->GetShader();
        shader->SetUniform("u_LightSpaceMtx", mLightSpaceMtx);
    }

    void ShadowPass::BindShadowMap(uint32_t texUnit) const {
      GLCALL(glBindTextureUnit(texUnit, GetShadowMapBuffer()));
    }

} // namespace Graphics
