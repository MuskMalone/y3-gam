#pragma once
#include <memory>
#include <vector>
#include <Core/Object.h>
#include <glm/glm.hpp>

// Spatial Partitioning
namespace SP
{

  struct ProxyObj
  {
    using Pointer = std::shared_ptr<ProxyObj>;
    using ObjPointer = std::shared_ptr<Object>;

    ObjPointer data;
    Pointer next;
    
    ProxyObj() : data{}, next{} {}
    ProxyObj(ObjPointer const& _data, Pointer const& _next = nullptr) : data{ _data }, next{ _next } {}

    // helper to retrieve object's vertices
    std::vector<glm::vec3> const& GetVertices() const noexcept { return data->meshRef->GetVertexAttribs().points; }
    std::vector<GLushort> const& GetIndices() const noexcept { return data->meshRef->GetVertexAttribs().indices; }
    void AddObject(Pointer const& obj) {
      if (!data) {
        data = obj->data;
        return;
      }

      next = obj;
    }
  };

} // namespace SP
