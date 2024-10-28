#include <pch.h>
#include "GeomPass.h"
#include "Core/Entity.h"
#include <Graphics/EditorCamera.h>
#include <Core/Components/Components.h>
#include "Color.h"
#include "Asset/IGEAssets.h"

namespace Graphics {
  using EntityXform = std::pair<ECS::Entity, glm::mat4>;
  using MatGroup = std::vector<EntityXform>;
  using MatGroupsMap = std::unordered_map<uint32_t, MatGroup>;

  GeomPass::GeomPass(const RenderPassSpec& spec) : RenderPass(spec) {

  }

  void GeomPass::Render(EditorCamera const& cam, std::vector<ECS::Entity> const& entities) {
    Begin();
    //auto shader = mSpec.pipeline->GetShader();

    // Use a map to group entities by material ID
    MatGroupsMap matGroups;

    for (ECS::Entity const& entity : entities) {
      if (!entity.HasComponent<Component::Mesh>()) { continue; }

      auto const& xform = entity.GetComponent<Component::Transform>();
      auto const& mesh = entity.GetComponent<Component::Mesh>();

      // Skip if mesh is null
      if (!mesh.meshSource.IsValid()) {
        continue;
      }

      uint32_t matID = 0;
      if (entity.HasComponent<Component::Material>()) {
        auto const& matComponent = entity.GetComponent<Component::Material>();
        matID = matComponent.matIdx;
      }
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
        Graphics::Renderer::SubmitInstance(mesh.meshSource, worldMtx, Color::COLOR_WHITE, entity.GetEntityID(), matID);
      }
      mSpec.pipeline->GetSpec().instanceLayout;
      // Flush all collected instances and render them in a single draw call
      Renderer::RenderInstances();
    }

    End();
  }

} // namespace Graphics
