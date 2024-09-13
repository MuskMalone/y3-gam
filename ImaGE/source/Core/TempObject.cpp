#include <pch.h>
#include "TempObject.h"
#include <glm/gtx/transform.hpp>
#include <Graphics/MeshGen.h>
#include <memory>

Object::Object(Graphics::MeshType type, glm::vec4 const& color,
  glm::vec3 const& _pos, glm::vec3 const& _scale)
  : mdlTransform{
    1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f
  }, clr{ color }, transform{ _pos, _scale },
  modified{ true }, collided{ false }
{
  meshRef = Graphics::MeshGen::GetMesh(type);
}

Object::Object(std::string const& filePath, glm::vec3 const& _pos, glm::vec3 const& _scale)
  : mdlTransform{
  1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f
  }, transform{ _pos, _scale },
  clr{ 1.f, 112.f / 255.f, 153.f / 255.f, 1.f },
  modified{ true }, collided{ false }
{
  meshRef = Graphics::MeshGen::GetMesh(filePath);
  Update(0.f);
}


void Object::Update(float deltaTime)
{
  // if values weren't touched, we don't have to recompute
  if (!modified) { return; }

  //scale
  mdlTransform = {
    transform.worldScale.x, 0.f, 0.f, 0.f,
    0.f,  transform.worldScale.y, 0.f, 0.f,
    0.f, 0.f,  transform.worldScale.z, 0.f,
    0.f, 0.f, 0.f, 1.f
  };

  // rotate
  mdlTransform = glm::rotate(mdlTransform, glm::radians(transform.worldRot.x), glm::vec3(1.f, 0.f, 0.f));
  mdlTransform = glm::rotate(mdlTransform, glm::radians(transform.worldRot.y), glm::vec3(0.f, 1.f, 0.f));
  mdlTransform = glm::rotate(mdlTransform, glm::radians(transform.worldRot.z), glm::vec3(0.f, 0.f, 1.f));

  // translate
  mdlTransform[3] = glm::vec4(transform.worldPos, 1.f);

  modified = false;
}
