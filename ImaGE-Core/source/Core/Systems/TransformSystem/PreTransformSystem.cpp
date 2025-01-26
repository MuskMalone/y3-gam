/*!*********************************************************************
\file   PreTransformSystem.cpp
\author chengen.lau\@digipen.edu
\date   03-October-2024
\brief  Computes the world transform of entities based on their local
        transform.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "PreTransformSystem.h"
#include <glm/gtx/euler_angles.hpp>
#include <Core/Components/Transform.h>

namespace Systems {

  using Component::Transform;

  void PreTransformSystem::Update() {
    for (ECS::Entity entity : mEntityManager.GetAllEntitiesWithComponents<Transform>()) {

      if (mEntityManager.HasParent(entity)) { continue; }

      Transform& transform{ entity.GetComponent<Transform>() };
      bool const modified{ transform.modified };

      if (modified) {
        transform.worldPos = transform.position;
        transform.worldScale = transform.scale;
        transform.worldRot = transform.rotation;

        transform.ComputeWorldMtx();
        transform.modified = false;
      }

      if (!mEntityManager.HasChild(entity)) { continue; }

      // update children
      for (ECS::Entity& child : mEntityManager.GetChildEntity(entity)) {
        UpdateLocalTransform(child, modified);
      }
    }
  }

  void PreTransformSystem::UpdateLocalTransform(ECS::Entity entity, bool parentModified) {
    Transform& trans{ entity.GetComponent<Transform>() };
    bool modified{ false };

    // dont bother computing if nothing changed
    if (trans.modified || parentModified) {
      Transform const& parentTrans{ mEntityManager.GetParentEntity(entity).GetComponent<Transform>() };

      // update local to world with parent xform
      trans.worldPos = parentTrans.worldMtx * glm::vec4(trans.position, 1.f);
      trans.worldRot = /*glm::normalize*/(parentTrans.worldRot * trans.rotation);
      trans.worldScale = parentTrans.worldScale * trans.scale;

      // compute the mtx
      trans.ComputeWorldMtx();

      // set flags
      trans.modified = false;
      modified = true;
    }

    if (!mEntityManager.HasChild(entity)) { return; }

    // do the same for all children
    for (ECS::Entity child : mEntityManager.GetChildEntity(entity)) {
      UpdateLocalTransform(child, modified);
    }
  }

} // namespace Systems
