/*!*********************************************************************
\file   Transform.h
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Definition of the Transform component. Contains local and
        world values for an entity, where local values are relative to
        its parent.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Component
{

  struct Transform
  {
    Transform() :
      worldMtx{ glm::identity<glm::mat4>() }, rotation{ 1.f, 0.f, 0.f, 0.f }, worldRot{ 1.f, 0.f, 0.f, 0.f },
      position{}, scale{ 1.f, 1.f, 1.f }, eulerAngles{}, worldPos{}, worldScale{ 1.f, 1.f, 1.f },
      modified{ true } {}
    Transform(glm::vec3 const& _pos, glm::vec3 const& _scale = { 1.f, 1.f, 1.f }, glm::vec3 const& _rot = {}) :
      worldMtx{ glm::identity<glm::mat4>() }, rotation{ 1.f, 0.f, 0.f, 0.f }, worldRot{ 1.f, 0.f, 0.f, 0.f },
      position{ _pos }, scale{ _scale }, eulerAngles{}, worldPos{ _pos }, worldScale{ _scale },
      modified{ true } {}

    // local setters
    inline void SetPosition(glm::vec3 const& newPos) noexcept { position = newPos; modified = true; }
    inline void SetWorldScale(glm::vec3 const& newScale) noexcept { scale = newScale; modified = true; }

    glm::mat4 GetLocalMdlMatrix() const;

    /*!*********************************************************************
    \brief
      Sets the local quaternion based on euler angles
    \param degrees
      The euler angles in degrees
    ************************************************************************/
    void SetLocalRotWithEuler(glm::vec3 const& degrees);

    /*!*********************************************************************
    \brief
      Rotates the transform by a specified amount on a given axis
    ************************************************************************/
    void ApplyWorldRotation(float angle, glm::vec3 const& axis);

    /*!*********************************************************************
    \brief
      Gets the world euler angles
    \return
      The world euler angles in the form of a vec3
    ************************************************************************/
    glm::vec3 GetWorldEulerAngles() const;

    /*!*********************************************************************
    \brief
      Helper function to set the local values to world. Used when an entity
      is unparented.
    ************************************************************************/
    void SetLocalToWorld() noexcept;

    /*!*********************************************************************
    \brief
      Computes the world matrix based on its world position, scale and
      rotation
    ************************************************************************/
    void ComputeWorldMtx();

    void ResetLocal();

    /*!*********************************************************************
    \brief
      Resets the component to default values
    ************************************************************************/
    void Clear() noexcept;

    glm::mat4 worldMtx;
    glm::quat rotation, worldRot;

    glm::vec3 position, scale, eulerAngles;  // local
    glm::vec3 worldPos, worldScale;
    bool modified;
  };

}
