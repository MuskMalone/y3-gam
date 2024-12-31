/*!*********************************************************************
\file   TransformHelpers.h
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Contains the definitions of helper functions dealing with
        updating transforms of entities.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "TransformHelpers.h"
#include <Core/Components/Transform.h>
#include <Core/Components/Tag.h>

namespace {

}

namespace TransformHelpers {

  void UpdateLocalTransform(ECS::Entity entity) {
    if (ECS::EntityManager::GetInstance().HasParent(entity)) {
      UpdateTransformToNewParent(entity);
    }
    else {
      Component::Transform& trans{ entity.GetComponent<Component::Transform>() };
      trans.SetLocalToWorld();
      trans.ComputeWorldMtx();
    }
  }

  void UpdateWorldTransform(ECS::Entity entity) {
    ECS::EntityManager& em{ ECS::EntityManager::GetInstance() };
    Component::Transform& trans{ entity.GetComponent<Component::Transform>() };

    if (em.HasParent(entity)) {
      // update world with parent xform
      Component::Transform& parentTrans{ em.GetParentEntity(entity).GetComponent<Component::Transform>() };
      trans.parentWorldMtx = parentTrans.worldMtx;
      trans.worldPos = parentTrans.worldMtx * glm::vec4(trans.position, 1.f);
      trans.worldScale = parentTrans.worldScale * trans.scale;
      trans.worldRot = parentTrans.worldRot * trans.rotation;
      trans.ComputeWorldMtx();
    }
    else {
      trans.worldPos = trans.position;
      trans.worldScale = trans.scale;
      trans.worldRot = trans.rotation;
      trans.ComputeWorldMtx();
    }

    if (em.HasChild(entity)) {
      for (ECS::Entity child : em.GetChildEntity(entity)) {
        UpdateWorldTransformRecursive(child);
      }
    }
  }

  void SetEntityWorldPos(ECS::Entity entity, glm::vec3 const& newPos) {
    ECS::EntityManager& em{ ECS::EntityManager::GetInstance() };
    Component::Transform& trans{ entity.GetComponent<Component::Transform>() };

    trans.worldPos = newPos;
    trans.ComputeWorldMtx();
    trans.modified = true;

    if (em.HasChild(entity)) {
      for (ECS::Entity child : em.GetChildEntity(entity)) {
        child.GetComponent<Component::Transform>().modified = true;
      }
    }

    if (em.HasParent(entity)) {
      // update local with inverse of parent xform
      Component::Transform& parentTrans{ em.GetParentEntity(entity).GetComponent<Component::Transform>() };
      trans.position = glm::inverse(parentTrans.worldMtx) * glm::vec4(trans.worldPos, 1.f);

      // due to floating point precision, explicitly check
      // if quat has been rounded < 1.f to prevent NaN euler values
      if (glm::abs(glm::length2(trans.worldRot) - 1.f) > glm::epsilon<float>()) {
        trans.rotation = glm::normalize(glm::inverse(parentTrans.worldRot) * trans.worldRot);
        //trans.eulerAngles = glm::degrees(glm::eulerAngles(trans.rotation));
      }
      else {
        trans.rotation = /*glm::normalize*/(glm::inverse(parentTrans.worldRot) * trans.worldRot);
        //trans.eulerAngles = glm::degrees(glm::eulerAngles(trans.rotation));
      }
      trans.scale = trans.worldScale / parentTrans.worldScale;
    }
    else {
      trans.position = newPos;
    }
  }

  void SetEntityWorldScale(ECS::Entity entity, glm::vec3 const& newScale) {
    ECS::EntityManager& em{ ECS::EntityManager::GetInstance() };
    Component::Transform& trans{ entity.GetComponent<Component::Transform>() };

    trans.worldScale = newScale;
    trans.ComputeWorldMtx();
    trans.modified = true;

    if (em.HasChild(entity)) {
      for (ECS::Entity child : em.GetChildEntity(entity)) {
        UpdateWorldTransformRecursive(child);
      }
    }

    if (em.HasParent(entity)) {
      UpdateTransformToNewParent(entity);
    }
    else {
      trans.scale = newScale;
    }
  }

  void UpdateTransformToNewParent(ECS::Entity entity) {
    ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };
    Component::Transform& trans{ entity.GetComponent<Component::Transform>() };

    Component::Transform& parentTrans{ entityMan.GetParentEntity(entity).GetComponent<Component::Transform>() };

    if (parentTrans.scale.x == 0.f || parentTrans.scale.y == 0.f || parentTrans.scale.z == 0.f) {
      throw Debug::Exception<Component::Transform>(Debug::LVL_CRITICAL,
        Msg("Entity " + entityMan.GetParentEntity(entity).GetComponent<Component::Tag>().tag + "'s scale is 0!"));
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

    // if no children, we are done
    if (!entityMan.HasChild(entity)) { return; }

    // update all children transforms
    for (ECS::Entity& child : entityMan.GetChildEntity(entity)) {
      UpdateTransformToNewParent(child);
    }
  }

  void UpdateWorldTransformRecursive(ECS::Entity entity) {
    ECS::EntityManager& em{ ECS::EntityManager::GetInstance() };
    Component::Transform& trans{ entity.GetComponent<Component::Transform>() },
      & parentTrans{ em.GetParentEntity(entity).GetComponent<Component::Transform>() };

    // recalculate the world position, then updates its own matrix
    trans.parentWorldMtx = parentTrans.worldMtx;
    trans.worldPos = parentTrans.worldMtx * glm::vec4(trans.position, 1.f);
    trans.worldScale = parentTrans.worldScale * trans.scale;
    trans.worldRot = parentTrans.worldRot * trans.rotation;
    trans.ComputeWorldMtx();

    if (!em.HasChild(entity)) { return; }

    for (ECS::Entity child : em.GetChildEntity(entity)) {
      UpdateWorldTransformRecursive(child);
    }
  }

} // namespace TransformHelpers

namespace {

  
}
