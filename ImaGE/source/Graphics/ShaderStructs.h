#pragma once
#include <glm/glm.hpp>

namespace Graphics
{

  struct Light
  {
    glm::vec3 position;
    glm::vec3 ambientIntensity, diffuseIntensity, specularIntensity;

    void SetUniforms(ShaderProgram& shader) const;
  };

  struct Material
  {
    glm::vec3 ambient, diffuse, specular;
    float shininess;

    void SetUniforms(ShaderProgram& shader) const;
  };

} // namespace Graphics