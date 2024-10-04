#include <pch.h>
#include "LocalToWorldTransformSystem.h"
#include <glm/gtx/euler_angles.hpp>
#include <Core/Components/Transform.h>

namespace Systems {

  using Component::Transform;

  void LocalToWorldTransformSystem::Update() {
    for (ECS::Entity entity : mEntityManager.GetAllEntitiesWithComponents<Transform>()) {

      if (mEntityManager.HasParent(entity)) { continue; }

      {
        Transform& transform{ entity.GetComponent<Transform>() };
        if (transform.modified) {
          transform.worldPos = transform.position;
          transform.worldScale = transform.scale;
          transform.worldRot = transform.rotation;

          transform.ComputeWorldMtx();
          transform.modified = false;

          // if no children, we are done
          if (mEntityManager.HasChild(entity)) {
            // force each child to update their transform
            for (ECS::Entity& child : mEntityManager.GetChildEntity(entity)) {
              child.GetComponent<Transform>().modified = true;
            }
          }

          transform.modified = false;
        }
      }

      // update mtx
      UpdateLocalToWorld(entity);
    }
  }

  void LocalToWorldTransformSystem::UpdateLocalToWorld(ECS::Entity entity) {
    Transform& trans{ entity.GetComponent<Transform>() };

    // dont bother computing if nothing changed
    if (trans.modified) {
      Transform& parentTrans{ mEntityManager.GetParentEntity(entity).GetComponent<Transform>() };

      // update local to world with parent xform
      trans.parentWorldMtx = parentTrans.worldMtx;
      trans.worldPos = trans.parentWorldMtx * glm::vec4(trans.position, 1.f);
      trans.worldRot = /*glm::normalize*/(parentTrans.worldRot * trans.rotation);
      trans.worldScale = parentTrans.worldScale * trans.scale;

      // compute the mtx
      trans.ComputeWorldMtx();
    }

    // if no children, we are done
    if (!mEntityManager.HasChild(entity)) { 
      trans.modified = false;
      return;
    }

    // else if transform was modified, update all children transforms
    if (trans.modified) {
      for (ECS::Entity& child : mEntityManager.GetChildEntity(entity)) {
        child.GetComponent<Transform>().modified = true;
        UpdateLocalToWorld(child);
      }

      trans.modified = false;
    }
    else {
      for (ECS::Entity& child : mEntityManager.GetChildEntity(entity)) {
        UpdateLocalToWorld(child);
      }
    }
  }

} // namespace Systems
