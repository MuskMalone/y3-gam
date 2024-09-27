#pragma once
#include <vector>
#include <glad/glad.h>

struct aiMesh;

namespace Graphics
{

  struct VertexAttribs
  {
    std::vector<GLushort> indices;
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> normals;
    std::vector <glm::vec2> texCoords;

    VertexAttribs() = default;
    void AddMeshes(aiMesh const* mesh);  // adds mesh data into existing vertices
  };

  VertexAttribs GeneratePoint();
  VertexAttribs GenerateLine();
  VertexAttribs GenerateTriangle();
  VertexAttribs GeneratePlane(int slices = 30);
  VertexAttribs GenerateCube(GLfloat size = 1.0f);
  VertexAttribs GenerateSphere(float rad = 1.0f, GLuint sl = 20, GLuint st = 20);

} // namespace Graphics