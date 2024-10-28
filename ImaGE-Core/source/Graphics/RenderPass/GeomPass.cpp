#include <pch.h>
#include "GeomPass.h"
#include "Core/Entity.h"
#include <Graphics/EditorCamera.h>
#include <Core/Components/Components.h>
#include "Color.h"
#include "Asset/IGEAssets.h"

namespace Graphics {

  GeomPass::GeomPass(const RenderPassSpec& spec) : RenderPass(spec) {

  }

  void GeomPass::Render(EditorCamera const& cam) {
    Begin();

    auto shader = mSpec.pipeline->GetShader();
    shader->SetUniform("u_ViewProjMtx", cam.GetViewProjMatrix());
    shader->SetUniform("u_CamPos", cam.GetPosition());

    //@TODO in future add light + materials

    //Renderer::BeginBatch();
    for (ECS::Entity entity : ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::Transform, Component::Mesh>()) {
      auto const& xform = entity.GetComponent<Component::Transform>();
      auto const& mesh = entity.GetComponent<Component::Mesh>();

      // Skip if mesh is null
      if (!mesh.meshSource.IsValid())
        continue;

      std::vector<GLuint64> hdls;
      for (auto const& map : Renderer::GetAlbedoMaps()) {
        hdls.push_back(IGE_REF(IGE::Assets::TextureAsset, map)->mTexture.GetBindlessHandle()); //should not be here TODO move somewhere else
      }
      if (hdls.size() > 0)
        shader->SetUniform("u_AlbedoMap", hdls.data(), static_cast<unsigned int>(hdls.size()));

      hdls.clear();
      for (auto const& map : Renderer::GetNormalMaps()) {
        hdls.push_back(IGE_REF(IGE::Assets::TextureAsset, map)->mTexture.GetBindlessHandle()); //should not be here TODO move somewhere else
      }
      if (hdls.size() > 0)
        shader->SetUniform("u_NormalMap", hdls.data(), static_cast<unsigned int>(hdls.size()));

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
        shader->SetUniform("u_Albedo", glm::vec3(0.5, 0.5, 0.5));
        shader->SetUniform("u_Metalness", 0.0f);
        shader->SetUniform("u_Roughness", 0.0f);
        shader->SetUniform("u_Transparency", 1.0f);
        shader->SetUniform("u_AO", 1.f);
        //shader->SetUniform("u_AlbedoMap", Renderer::GetWhiteTexture(), 0);
      }

      //Graphics::Renderer::SubmitMesh(mesh.mesh, xform.worldPos, xform.worldRot, xform.worldScale, { 1.f, 1.f, 1.f, 1.f }); //@TODO: adjust color and rotation as needed
      Graphics::Renderer::SubmitInstance(mesh.meshSource, xform.worldMtx, Color::COLOR_WHITE, entity.GetEntityID(), matID);
    }

    mSpec.pipeline->GetSpec().instanceLayout;
    // Flush all collected instances and render them in a single draw call
    Renderer::RenderInstances();
    //Renderer::FlushBatch(shared_from_this());

    End();
  }

} // namespace Graphics
