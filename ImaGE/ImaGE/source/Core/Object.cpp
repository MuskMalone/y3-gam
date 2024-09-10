#include <pch.h>
#include "Object.h"
#include <glm/gtx/transform.hpp>
#include <Graphics/MeshGen.h>
#include <BoundingVolumes/BoundingSphere.h>
#include <BoundingVolumes/AABB.h>
#include <BoundingVolumes/Extra/OBB.h>
#include <BoundingVolumes/Extra/Ellipsoid.h>
#include <memory>

Object::Object(Graphics::MeshType type, glm::vec4 const& color,
  glm::vec3 const& _pos, glm::vec3 const& _scale)
  : mdlTransform{
    1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f
  },
  collider{}, clr{ color }, transform{ _pos, _scale },
  modified{ true }, collided{ false }
{

  if (Settings::bvType == BV::BVType::AABB) {
    collider = std::make_shared<BV::AABB>(transform.pos, transform.scale);
  }
  else if (Settings::bvType == BV::BVType::OBB) {
    collider = std::make_shared<BV::OBB>(transform.pos, transform.scale);
  }
  else if (Settings::bvType == BV::BVType::ELLIPSOID) {
    collider = std::make_shared<BV::Ellipsoid>(transform.pos, transform.scale);
  }
  else {
    collider = std::make_shared<BV::BoundingSphere>(transform.pos,
      glm::max(transform.scale.x, glm::max(transform.scale.y, transform.scale.z)));
  }
  meshRef = Graphics::MeshGen::GetMesh(type);
  collider->ComputeBV(meshRef->GetVertexAttribs());
}

Object::Object(std::string const& filePath, BV::BVType type, glm::vec3 const& _pos, glm::vec3 const& _scale)
  : mdlTransform{
  1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f
  }, transform{ _pos, _scale },
  collider{}, clr{ 1.f, 112.f / 255.f, 153.f / 255.f, 1.f },
  modified{ true }, collided{ false }
{
  if (type == BV::BVType::AABB) {
    collider = std::make_shared<BV::AABB>(transform.pos, transform.scale);
  }
  else {
    collider = std::make_shared<BV::BoundingSphere>(transform.pos,
      glm::max(transform.scale.x, glm::max(transform.scale.y, transform.scale.z)));
  }
  meshRef = Graphics::MeshGen::GetMesh(filePath);
  collider->ComputeBV(meshRef->GetVertexAttribs());
  Update(0.f);
}


void Object::Update(float deltaTime)
{
  // if values weren't touched, we don't have to recompute
  if (!modified) { return; }

  glm::vec3 const combinedScale{ transform.GetBaseScale() * transform.scale };

  //scale
  mdlTransform = {
    combinedScale.x, 0.f, 0.f, 0.f,
    0.f,  combinedScale.y, 0.f, 0.f,
    0.f, 0.f,  combinedScale.z, 0.f,
    0.f, 0.f, 0.f, 1.f
  };

  // rotate
  mdlTransform = glm::rotate(mdlTransform, glm::radians(transform.rot.x), glm::vec3(1.f, 0.f, 0.f));
  mdlTransform = glm::rotate(mdlTransform, glm::radians(transform.rot.y), glm::vec3(0.f, 1.f, 0.f));
  mdlTransform = glm::rotate(mdlTransform, glm::radians(transform.rot.z), glm::vec3(0.f, 0.f, 1.f));

  // translate
  mdlTransform[3] = glm::vec4(transform.pos, 1.f);

  // update the bounding vol
  if (collider) {
    collider->ComputeBV(meshRef->GetVertexAttribs());
    collider->Update(transform);
  }

  modified = false;
}

void Object::ChangeBV(BV::BVType type)
{
  switch (type)
  {
  case BV::BVType::AABB:
    collider = std::make_shared<BV::AABB>(transform.pos, transform.scale);
    break;
  case BV::BVType::OBB:
    collider = std::make_shared<BV::OBB>(transform.pos, transform.scale);
    break;
  case BV::BVType::ELLIPSOID:
    collider = std::make_shared<BV::Ellipsoid>(transform.pos, transform.scale);
    break;
  case BV::BVType::RITTER:
  case BV::BVType::LARSSON:
  case BV::BVType::PCA:
    collider = std::make_shared<BV::BoundingSphere>(transform.pos, transform.scale);
    break;
  default:
    throw std::runtime_error("Trying to change to invalid BV type!");
    break;
  }

  collider->ComputeBV(meshRef->GetVertexAttribs());
  //collider->Update(transform);
  modified = true;
}

void Object::Reset()
{
  transform.Clear();
  collider->Reset();
  modified = true;
}
