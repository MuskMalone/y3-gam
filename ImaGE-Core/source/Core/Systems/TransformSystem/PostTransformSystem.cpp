/*!*********************************************************************
\file   PostTransformSystem.cpp
\author chengen.lau\@digipen.edu
\date   03-October-2024
\brief  Computes the local transform of entities based on their world
        transform. Should only run after world transform is updated.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "PostTransformSystem.h"
#include <Core/Components/Transform.h>
#include <Core/Systems/TransformSystem/TransformHelpers.h>

namespace Systems {

  using Component::Transform;

  void PostTransformSystem::Update() {
    for (ECS::Entity entity : mEntityManager.GetAllEntitiesWithComponents<Transform>()) {

      if (mEntityManager.HasParent(entity)) { continue; }

      Transform& transform{ entity.GetComponent<Transform>() };
      bool const modified{ transform.modified };
      if (modified) {
        transform.position = transform.worldPos;
        transform.scale = transform.worldScale;
        transform.rotation = transform.worldRot;
        transform.eulerAngles = glm::degrees(glm::eulerAngles(transform.rotation));

        transform.modified = false;
      }

      if (!mEntityManager.HasChild(entity)) { continue; }

      // update children
      for (ECS::Entity& child : mEntityManager.GetChildEntity(entity)) {
        UpdateWorldTransform(child, modified);
      }
    }
  }

  void PostTransformSystem::UpdateWorldTransform(ECS::Entity entity, bool parentModified) {
    Transform& trans{ entity.GetComponent<Transform>() };
    bool modified{ false };

    // parent modified: update world based on local
    if (parentModified) {
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
    // modified but parent wasn't: update local using world to match parent
    else if (trans.modified) {
      if (trans.scale.x == 0.f || trans.scale.y == 0.f || trans.scale.z == 0.f) {
        throw Debug::Exception<PostTransformSystem>(Debug::LVL_CRITICAL,
          Msg("Entity " + entity.GetTag() + "'s scale is 0!"));
      }

      Transform const& parentTrans{ mEntityManager.GetParentEntity(entity).GetComponent<Transform>() };

      // update local with inverse of parent xform
      trans.position = glm::inverse(parentTrans.worldMtx) * glm::vec4(trans.worldPos, 1.f);
      // due to floating point precision, explicitly check
      // if quat has been rounded < 1.f to prevent NaN euler values
      if (glm::abs(glm::length2(trans.worldRot) - 1.f) > glm::epsilon<float>()) {
        trans.rotation = glm::normalize(glm::inverse(parentTrans.worldRot) * trans.worldRot);
      }
      else {
        trans.rotation = glm::normalize(glm::inverse(parentTrans.worldRot) * trans.worldRot);
      }
      trans.scale = trans.worldScale / parentTrans.worldScale;

      // set flags
      trans.modified = false;
      modified = true;
    }

    if (!mEntityManager.HasChild(entity)) { return; }

    // do the same for all children
    for (ECS::Entity child : mEntityManager.GetChildEntity(entity)) {
      UpdateWorldTransform(child, modified);
    }
  }

} // namespace Systems
