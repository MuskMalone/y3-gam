#pragma once

#include "Vertex.h"
#include "VertexArray.h"
#include "Submesh.h"


namespace Graphics {
	class MeshSource {
    public:
        MeshSource(const std::shared_ptr<VertexArray>& vao, const std::vector<Submesh>& submeshes)
            : mVertexArray{ vao }, mSubmeshes{ submeshes } {}

        const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
        const std::vector<uint32_t>& GetIndices() const { return m_Indices; }

        const std::shared_ptr<VertexArray>& GetVertexArray() const { return mVertexArray; }
        const std::vector<Submesh>& GetSubmeshes() const { return mSubmeshes; }

        //const AABB& GetBoundingBox() const { return mBoundingBox; } TO ADD IN THE FUTURE

    private:
        std::vector<Vertex> m_Vertices;
        std::vector<uint32_t> m_Indices;

        std::shared_ptr<VertexArray> mVertexArray;
        std::vector<Submesh> mSubmeshes;

        //MATERIALS VECTOR TO ADD
        //AABB mBoundingBox add in the future
	};
}

