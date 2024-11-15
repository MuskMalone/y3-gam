#include <pch.h>
#include "MeshSource.h"
#include <Core/Entity.h>
#include <Core/Components/Mesh.h>
#include <Core/Components/Material.h>

namespace Graphics {
  ECS::Entity MeshSource::ConstructEntity(IGE::Assets::GUID const& guid, std::string const& fileName) const {
    ECS::EntityManager& em{ ECS::EntityManager::GetInstance() };

    if (mSubmeshes.empty()) {
      ECS::Entity entity{ em.CreateEntityWithTag(fileName) };
      entity.EmplaceComponent<Component::Mesh>(guid, fileName, true);
      entity.EmplaceComponent<Component::Material>();
      return entity;
    }

    ECS::Entity parent{ em.CreateEntityWithTag(fileName) };
    // if submeshes exist, create an entity for each one
    for (unsigned i{}; i < mSubmeshes.size(); ++i) {
      std::string const& name{ mMeshNames[i] };
      ECS::Entity entity{ em.CreateEntityWithTag(name) };
      entity.EmplaceComponent<Component::Mesh>(guid, name, true);
      entity.EmplaceComponent<Component::Material>();
      em.SetParentEntity(parent, entity);
    }

    return parent;
  }
}

