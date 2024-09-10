#pragma once
#include <glm/glm.hpp>
#include <Core/Transform.h>
#include <Graphics/Model.h>
#include <Graphics/MeshType.h>
#include <memory>

struct Object
{
  glm::mat4 mdlTransform;
  Transform transform;
  glm::vec4 clr;
  
  std::shared_ptr<Graphics::Model> meshRef;
  bool modified, collided;

  Object(Graphics::MeshType type, glm::vec4 const& color = { 1.f, 0.5f, 0.2f, 1.f },
    glm::vec3 const& _pos = {}, glm::vec3 const& _scale = { 1.f, 1.f, 1.f });

  Object(std::string const& filePath, glm::vec3 const& _pos = {},
    glm::vec3 const& _scale = { 1.f, 1.f, 1.f });

  void Update(float deltaTime);
  void Reset();

  inline glm::vec3& GetPos() noexcept { return transform.pos; }
  inline glm::vec3 const& GetPos() const noexcept { return transform.pos; }
  inline glm::vec3& GetScale() noexcept { return transform.scale; }
  inline glm::vec3 const& GetScale() const noexcept { return transform.scale; }
  inline glm::vec3 const& GetWorldPos() const noexcept { return transform.worldPos; }
};

