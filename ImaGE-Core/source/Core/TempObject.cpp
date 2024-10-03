#include <pch.h>
#include "TempObject.h"
#include <glm/gtx/transform.hpp>
#include <memory>

Object::Object(Graphics::MeshType type, glm::vec4 const& color,
  glm::dvec3 const& _pos, glm::dvec3 const& _scale)
  : mdlTransform{
    1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0
  }, clr{ color }, transform{ _pos, _scale },
  modified{ true }, collided{ false }
{
  
}

Object::Object(std::string const& filePath, glm::dvec3 const& _pos, glm::dvec3 const& _scale)
  : mdlTransform{
  1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0
  }, transform{ _pos, _scale },
  clr{ 1.f, 112.f / 255.f, 153.f / 255.f, 1.f },
  modified{ true }, collided{ false }
{
  Update(0.0);
}


void Object::Update([[maybe_unused]] float deltaTime)
{
  // if values weren't touched, we don't have to recompute
  if (!modified) { return; }

  //scale
  mdlTransform = {
    transform.worldScale.x, 0.0, 0.0, 0.0,
    0.0,  transform.worldScale.y, 0.0, 0.0,
    0.0, 0.0,  transform.worldScale.z, 0.0,
    0.0, 0.0, 0.0, 1.0
  };

  // rotate
  mdlTransform = glm::rotate(mdlTransform, glm::radians(static_cast<double>(transform.worldRot.x)), glm::dvec3(1.0, 0.0, 0.0));
  mdlTransform = glm::rotate(mdlTransform, glm::radians(static_cast<double>(transform.worldRot.y)), glm::dvec3(0.0, 1.0, 0.0));
  mdlTransform = glm::rotate(mdlTransform, glm::radians(static_cast<double>(transform.worldRot.z)), glm::dvec3(0.0, 0.0, 1.0));

  // translate
  mdlTransform[3] = glm::dvec4(transform.worldPos, 1.0);

  modified = false;
}
