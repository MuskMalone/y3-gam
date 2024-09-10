#include <pch.h>
#include "OctNode.h"
#include <Intersection/Primitives.h>

// Spatial Partitioning
namespace SP
{
  unsigned OctNode::clrIdx{};

  void OctNode::Initialize()
  {
    // create VAO
    GLuint vbo;
    glCreateBuffers(1, &vbo);

    glNamedBufferStorage(vbo, sizeof(glm::vec3) * vertices.size(), nullptr, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferSubData(vbo, 0, sizeof(glm::vec3) * vertices.size(), vertices.data());

    // vtx pos arr
    glCreateVertexArrays(1, &vao);

    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(glm::vec3));
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, 0, 0);

    glBindVertexArray(0); // unbind VAO

    ComputeBV();

    for (auto const& child : children)
    {
      if (child)
      {
        child->Initialize();
      }
    }
  }

  void OctNode::CreateChildren()
  {
    for (int i = 0; i < 8; ++i)
    {
      glm::vec3 offset;
      float const step = halfWidth * 0.5f;
      offset.x = ((i & 1) ? step : -step);
      offset.y = ((i & 2) ? step : -step);
      offset.z = ((i & 4) ? step : -step);
      children[i] = std::make_shared<OctNode>(center + offset, step);
    }
  }

  void OctNode::ComputeBV()
  {
    bv.worldPos = center;
    bv.worldHalfExtents = glm::vec3(halfWidth);
    bv.UpdateTransformWithWorld();
  }

  void OctNode::Draw(Graphics::ShaderProgram& shader)
  {
    shader.SetUniform("uVtxClr", drawClr);
    shader.SetUniform("uMdlTransform", glm::mat4(1.f));

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));

    if (Settings::displayTreeBV && !Empty())
    {
      bv.Draw(shader);
    }

    for (auto const& child : children)
    {
      if (child)
      {
        child->Draw(shader);
      }
    }
  }

  void OctNode::SetNodePlanes(Primitives::Plane (&planes)[6])
  {
    planes[0].normal = { -1.f, 0.f, 0.f, center.x - halfWidth };
    planes[1].normal = { 1.f, 0.f, 0.f, -center.x - halfWidth };
    planes[2].normal = { 0.f, -1.f, 0.f, center.y - halfWidth };
    planes[3].normal = { 0.f, 1.f, 0.f, -center.y - halfWidth };
    planes[4].normal = { 0.f, 0.f, -1.f, center.z - halfWidth };
    planes[5].normal = { 0.f, 0.f, 1.f, -center.z - halfWidth };
  }

  void OctNode::AddObject(ProxyObj::Pointer const& obj)
  {
    if (!objects)
    {
      objects = obj;
      return;
    }

    objects->AddObject(obj);
  }

} // namespace SP

