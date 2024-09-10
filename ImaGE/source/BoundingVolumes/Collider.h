#pragma once
#include <Core/Transform.h>
#include <Graphics/MeshType.h>
#include <Graphics/ShaderProgram.h>
#include <Graphics/Model.h>
#include <BoundingVolumes/BVTypes.h>
#include <memory>

namespace Graphics { class Model; }

namespace BV
{

  struct Collider
  {
    glm::mat4 transformMtx;
    glm::vec4 drawClr;
    std::shared_ptr<Graphics::Model> mesh;

    Collider() : transformMtx{}, drawClr{ 1.f, 1.f, 1.f, 0.75f }, mesh{ nullptr } {}
    virtual ~Collider() {}

    virtual void Update(Transform const& transform) = 0;
    virtual void Draw(Graphics::ShaderProgram& shader) const;
    virtual void Reset() = 0;

    void SetModel(Graphics::MeshType type);
    void SetModel(const char* type);
    virtual void ComputeBV(Graphics::VertexAttribs const& vtxAttribs) = 0;
  };

}