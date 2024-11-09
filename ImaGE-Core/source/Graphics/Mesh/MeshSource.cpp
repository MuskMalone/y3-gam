#include <pch.h>
#include "MeshSource.h"
#include <Core/Entity.h>
#include <Core/Components/Mesh.h>

namespace Graphics {
  ECS::Entity MeshSource::ConstructEntity(IGE::Assets::GUID const& guid, std::string const& fileName) const {
    ECS::EntityManager& em{ ECS::EntityManager::GetInstance() };

    if (mSubmeshes.empty()) {
      ECS::Entity entity{ em.CreateEntityWithTag(fileName) };
      entity.EmplaceComponent<Component::Mesh>(guid, fileName, true);
      return entity;
    }

    ECS::Entity parent{ em.CreateEntityWithTag(fileName) };
    // if submeshes exist, create an entity for each one
    for (unsigned i{}; i < mSubmeshes.size(); ++i) {
      ECS::Entity entity{ em.CreateEntityWithTag(fileName + std::to_string(i)) };
      entity.EmplaceComponent<Component::Mesh>(guid, fileName, true, i);
      em.SetParentEntity(parent, entity);
    }

    return parent;
  }
}

