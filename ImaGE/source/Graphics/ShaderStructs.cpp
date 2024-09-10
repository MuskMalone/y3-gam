#include <pch.h>
#include <Graphics/ShaderProgram.h>
#include "ShaderStructs.h"

namespace Graphics
{

  void Light::SetUniforms(ShaderProgram& shader) const
  {
    shader.SetUniform("light.position", position);
    shader.SetUniform("light.ambientIntensity", ambientIntensity);
    shader.SetUniform("light.diffuseIntensity", diffuseIntensity);
    shader.SetUniform("light.specularIntensity", specularIntensity);
  }

  void Material::SetUniforms(ShaderProgram& shader) const
  {
    shader.SetUniform("material.ambient", ambient);
    shader.SetUniform("material.diffuse", diffuse);
    shader.SetUniform("material.specular", specular);
    shader.SetUniform("material.shininess", shininess);
  }

  } // namespace Graphics
