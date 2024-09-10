#include <pch.h>
#include "Collider.h"
#include <Graphics/MeshGen.h>

namespace BV
{
  void Collider::SetModel(Graphics::MeshType type)
  {
    mesh = Graphics::MeshGen::GetMesh(type);
  }

  void Collider::SetModel(const char* type)
  {
    mesh = Graphics::MeshGen::GetMesh(type);
  }

  void Collider::Draw(Graphics::ShaderProgram& shader) const
  {
    shader.SetUniform("uVtxClr", drawClr);
    shader.SetUniform("uMdlTransform", transformMtx);

    auto const& mdl{ *mesh };
    glBindVertexArray(mdl.GetVAO());

    glDrawElements(mdl.primitiveType, static_cast<GLsizei>(mdl.drawCount), GL_UNSIGNED_SHORT, NULL);
  }
} // namespace BV
