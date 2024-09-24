#pragma once

namespace Graphics {
  struct Vertex
  {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
    float texIdx; // float as it is passed to shader
    glm::vec3 tangent;
    glm::vec3 bitangent;
    //glm::vec2 texcoord;
    glm::vec4 color;

    // Constructor with default values using curly braces
    //Vertex(glm::vec3 pos = { 0.0f, 0.0f, 0.0f },
    //    glm::vec3 norm = { 0.0f, 0.0f, 1.0f },
    //    glm::vec3 tan = { 1.0f, 0.0f, 0.0f },
    //    glm::vec3 bitan = { 0.0f, 1.0f, 0.0f },
    //    glm::vec2 tex = { 0.0f, 0.0f },
    //    glm::vec4 clr = { 1.0f, 1.0f, 1.0f, 1.0f })
    //    : position{ pos }, normal{ norm }, tangent{ tan }, bitangent{ bitan }, texcoord{ tex }, color{ clr }
    //{}
    Vertex(glm::vec3 pos = { 0.0f, 0.0f, 0.0f },
      glm::vec3 norm = { 0.0f, 0.0f, 1.0f },
      glm::vec2 tex = { 0.0f, 0.0f },
      glm::vec3 tan = { 1.0f, 0.0f, 0.0f },
      glm::vec3 bitan = { 0.0f, 1.0f, 0.0f },
      glm::vec4 clr = { 1.0f, 1.0f, 1.0f, 1.0f })
      : position{ pos }, normal{ norm }, texcoord{ tex }, tangent{ tan }, bitangent{ bitan }, color{ clr }
    {}
  };
} // namespace Graphics
