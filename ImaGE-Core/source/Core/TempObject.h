#pragma once
#include <glm/glm.hpp>
#include <Core/Component/Transform.h>
#include <Graphics/Model.h>
#include <Graphics/MeshType.h>
#include <memory>
#include <Core/Entity.h> //tch: also for testing
struct Object
{
  glm::dmat4 mdlTransform;
  ECS::Entity entity;
  Component::Transform transform;
  glm::vec4 clr;
  
  std::shared_ptr<Graphics::Model> meshRef;
  bool modified, collided;

  Object(Graphics::MeshType type, glm::vec4 const& color = { 1.f, 0.5f, 0.2f, 1.f },
    glm::dvec3 const& _pos = {}, glm::dvec3 const& _scale = { 1.0, 1.0, 1.0 });

  Object(std::string const& filePath, glm::dvec3 const& _pos = {},
    glm::dvec3 const& _scale = { 1.0, 1.0, 1.0 });

  void Update(float deltaTime);

  inline glm::vec3& GetPos() noexcept { return transform.worldPos; }
  inline glm::vec3 const& GetPos() const noexcept { return transform.worldPos; }
  inline glm::vec3& GetScale() noexcept { return transform.worldScale; }
  inline glm::vec3 const& GetScale() const noexcept { return transform.worldScale; }
};

