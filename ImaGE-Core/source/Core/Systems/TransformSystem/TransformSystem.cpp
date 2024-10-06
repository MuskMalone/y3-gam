/*!*********************************************************************
\file   TransformSystem.cpp
\author chengen.lau\@digipen.edu
\date   03-October-2024
\brief  Computes the local transform of entities based on their world
        transform. Should only run after world transform is updated.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "TransformSystem.h"
#include <Core/Components/Transform.h>
#include <Core/Systems/TransformSystem/TransformHelpers.h>

namespace Systems {

  using Component::Transform;

  void TransformSystem::Update() {
    for (ECS::Entity entity : mEntityManager.GetAllEntitiesWithComponents<Transform>()) {

      if (mEntityManager.HasParent(entity)) { continue; }

      {
        Transform& transform{ entity.GetComponent<Transform>() };
        if (transform.modified) {
          transform.position = transform.worldPos;
          transform.scale = transform.worldScale;
          transform.rotation = transform.worldRot;

          transform.ComputeWorldMtx();
          transform.modified = false;

          // if no children, we are done
          if (mEntityManager.HasChild(entity)) {
            // force each child to update their transform
            for (ECS::Entity& child : mEntityManager.GetChildEntity(entity)) {
              TransformHelpers::UpdateWorldTransformRecursive(child);
              child.GetComponent<Transform>().parentModified = true;
            }
          }
        }
      }

      // update mtx
      UpdateWorldToLocal(entity);
    }
  }

  void TransformSystem::UpdateWorldToLocal(ECS::Entity entity) {
    Transform& trans{ entity.GetComponent<Transform>() };
    
    // dont bother computing if nothing changed
    if (trans.modified || trans.parentModified) {
      Transform& parentTrans{ mEntityManager.GetParentEntity(entity).GetComponent<Transform>() };

      if (parentTrans.scale.x == 0.f || parentTrans.scale.y == 0.f || parentTrans.scale.z == 0.f) {
        throw Debug::Exception<TransformSystem>(Debug::LVL_CRITICAL,
          Msg("Entity " + mEntityManager.GetParentEntity(entity).GetComponent<Component::Tag>().tag + "'s scale is 0!"));
      }

      // update local with inverse of parent xform
      trans.position = glm::inverse(parentTrans.worldMtx) * glm::vec4(trans.worldPos, 1.f);
      // due to floating point precision, explicitly check
      // if quat has been rounded < 1.f to prevent NaN euler values
      if (glm::abs(glm::length2(trans.worldRot) - 1.f) > glm::epsilon<float>()) {
        trans.rotation = glm::normalize(glm::inverse(parentTrans.worldRot) * trans.worldRot);
        trans.eulerAngles = glm::degrees(glm::eulerAngles(trans.rotation));
      }
      else {
        trans.rotation = /*glm::normalize*/(glm::inverse(parentTrans.worldRot) * trans.worldRot);
        trans.eulerAngles = glm::degrees(glm::eulerAngles(trans.rotation));
      }
      trans.scale = trans.worldScale / parentTrans.worldScale;

      // don't have to update matrix here since world transform has already been computed
    }

    // if no children, we are done
    if (!mEntityManager.HasChild(entity)) {
      trans.modified = trans.parentModified = false;
      return;
    }

    // if root was modified, recursively call this function to re-update world transform
    if (trans.parentModified) {
      for (ECS::Entity& child : mEntityManager.GetChildEntity(entity)) {
        child.GetComponent<Transform>().modified = true;
        UpdateWorldToLocal(child);
      }

      trans.parentModified = false;
    }
    // else if transform was modified, update all children transforms
    else if (trans.modified) {
      trans.ComputeWorldMtx();
      for (ECS::Entity& child : mEntityManager.GetChildEntity(entity)) {
        TransformHelpers::UpdateWorldTransformRecursive(child);
      }

      trans.modified = false;
    }
    else {
      for (ECS::Entity& child : mEntityManager.GetChildEntity(entity)) {
        UpdateWorldToLocal(child);
      }
    }
  }

} // namespace Systems
