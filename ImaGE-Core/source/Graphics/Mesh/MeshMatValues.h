#pragma once
#include<glm/glm.hpp>

namespace Graphics {
  struct MeshMatValues {
    glm::vec3 ambientCol, diffuseCol, specularCol, emissiveCol;
    float shininess, refractionIndex, opacity;
  };
} // namespace Graphics
