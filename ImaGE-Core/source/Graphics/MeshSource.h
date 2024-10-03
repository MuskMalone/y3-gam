#pragma once
#include "Vertex.h"
#include "VertexArray.h"
#include "Submesh.h"

namespace Graphics {
	class MeshSource {
    public:
      MeshSource(const std::shared_ptr<VertexArray>& vao,
          const std::vector<Submesh>& submeshes,
          const std::vector<Vertex>& vertices,
          const std::vector<uint32_t>& indices)
          : mVertexArray{ vao }, mSubmeshes{ submeshes }, mVertices{ vertices }, mIndices{ indices } {}
      MeshSource(std::shared_ptr<VertexArray>&& vao,
        std::vector<Submesh>&& submeshes,
        std::vector<Vertex>&& vertices,
        std::vector<uint32_t>&& indices)
        : mVertexArray{ std::move(vao) }, mSubmeshes{ std::move(submeshes) }, mVertices{ std::move(vertices) }, mIndices{ std::move(indices) } {}

      const std::vector<Vertex>& GetVertices() const { return mVertices; }
      const std::vector<uint32_t>& GetIndices() const { return mIndices; }

      const std::shared_ptr<VertexArray>& GetVertexArray() const { return mVertexArray; }
      const std::vector<Submesh>& GetSubmeshes() const { return mSubmeshes; }

      //const AABB& GetBoundingBox() const { return mBoundingBox; } TO ADD IN THE FUTURE

    private:
      std::vector<Vertex> mVertices;
      std::vector<uint32_t> mIndices;

      std::shared_ptr<VertexArray> mVertexArray;
      std::vector<Submesh> mSubmeshes;

      //MATERIALS VECTOR TO ADD
      //AABB mBoundingBox add in the future
	};
}

