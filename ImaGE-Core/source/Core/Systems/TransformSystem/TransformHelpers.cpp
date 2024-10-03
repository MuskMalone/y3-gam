#include <pch.h>
#include "TransformHelpers.h"
#include <Core/Components/Transform.h>
#include <Core/Components/Tag.h>

namespace TransformHelpers {

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
    if (glm::length2(trans.worldRot) < 1.f) {
      trans.rotation = { 1.f, 0.f, 0.f, 0.f };
      trans.eulerAngles = {};
    }
    else {
      trans.rotation = /*glm::normalize*/(glm::inverse(parentTrans.worldRot) * trans.worldRot);
      trans.eulerAngles = glm::degrees(glm::eulerAngles(trans.rotation));
    }
    trans.scale = trans.worldScale / parentTrans.worldScale;

    // if no children, we are done
    if (!entityMan.HasChild(entity)) { return; }

    //trans.modified = false;
    // update all children transforms
    for (ECS::Entity& child : entityMan.GetChildEntity(entity)) {
      UpdateTransformToNewParent(child);
    }
  }

} // namespace TransformHelpers
