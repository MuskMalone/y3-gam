#include <pch.h>
#include "TransformSystem.h"
#include <glm/gtx/euler_angles.hpp>

namespace Systems {

  using Component::Transform;

  void TransformSystem::UpdateTransform(ECS::Entity entity) {
    Transform& trans{ entity.GetComponent<Transform>() };
    
    // dont bother computing if nothing changed
    if (trans.modified) {
      Transform& parentTrans{ mEntityManager.GetParentEntity(entity).GetComponent<Transform>() };

      // update local to world with parent xform
      trans.parentWorldMtx = parentTrans.worldMtx;
      trans.worldPos = trans.parentWorldMtx * glm::vec4(trans.position, 1.f);
      trans.worldScale = parentTrans.worldScale * trans.scale;
      trans.worldRot = parentTrans.worldRot + trans.rotation; // @TODO: combine properly in future

      // compute the mtx
      glm::mat4 translate{ glm::translate(glm::mat4{ 1.f }, trans.worldPos) };
      glm::mat4 rot{ glm::yawPitchRoll(trans.worldRot.y, trans.worldRot.x, trans.worldRot.z) };
      glm::mat4 scale{ glm::scale(glm::mat4{ 1.f }, trans.worldScale) };
      trans.worldMtx = translate * rot * scale;
    }

    // if no children, we are done
    if (!mEntityManager.HasChild(entity)) { return; }

    // if transform was modified, update all children transforms
    if (trans.modified) {
      for (ECS::Entity& child : mEntityManager.GetChildEntity(entity)) {
        child.GetComponent<Transform>().modified = true;
        UpdateTransform(child);
      }

      trans.modified = false;
    }
    else {
      for (ECS::Entity& child : mEntityManager.GetChildEntity(entity)) {
        UpdateTransform(child);
      }
    }
  }

  void TransformSystem::Update() {
    for (ECS::Entity entity : mEntityManager.GetAllEntitiesWithComponents<Transform>()) {

      if (mEntityManager.HasParent(entity)) { continue; }

      {
        Transform& transform{ entity.GetComponent<Transform>() };
        if (transform.modified) {
          transform.worldPos = transform.position;
          transform.worldScale = transform.scale;
          transform.worldRot = transform.rotation;

          glm::mat4 trans{ glm::translate(glm::mat4{ 1.f }, transform.worldPos) };
          glm::mat4 rot{ glm::yawPitchRoll(transform.worldRot.y, transform.worldRot.x, transform.worldRot.z) };
          glm::mat4 scale{ glm::scale(glm::mat4{ 1.f }, transform.worldScale) };

          transform.worldMtx = trans * rot * scale;
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
      UpdateTransform(entity);
    }
  }

} // namespace Systems
