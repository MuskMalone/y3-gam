/*!*********************************************************************
\file   MeshSource.h
\author k.choa\@digipen.edu
\date   22/09/2024
\brief  The MeshSource class contains the necessary vertex data, indices, and submesh information
        for a 3D model. It holds references to Vertex Array Objects (VAO) for rendering and can
        manage multiple submeshes. This class is designed to act as a source or blueprint for
        rendering 3D models.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <Graphics/Vertex.h>
#include <Graphics/VertexArray.h>
#include <Graphics/Mesh/Submesh.h>

// forward declarations
namespace ECS { class Entity; }
namespace IGE::Core { template <typename T> class GUID; }
namespace IGE::Assets { struct AssetGUIDTag; using GUID = IGE::Core::GUID<AssetGUIDTag>; }

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
        : mVertices{ std::move(vertices) }, mIndices{ std::move(indices) },
          mVertexArray{ std::move(vao) }, mSubmeshes{ std::move(submeshes) } {}

      const std::vector<Vertex>& GetVertices() const { return mVertices; }
      const std::vector<uint32_t>& GetIndices() const { return mIndices; }

      const std::shared_ptr<VertexArray>& GetVertexArray() const { return mVertexArray; }
      const std::vector<Submesh>& GetSubmeshes() const { return mSubmeshes; }

      ECS::Entity ConstructEntity(IGE::Assets::GUID const& guid, std::string const& fileName) const;

      //const AABB& GetBoundingBox() const { return mBoundingBox; } TO ADD IN THE FUTURE
      bool IsWireframe() { return mIsWireframe; }
      void ToggleWireframe() { mIsWireframe = !mIsWireframe; }
    private:
      std::vector<Vertex> mVertices;
      std::vector<uint32_t> mIndices;

      std::shared_ptr<VertexArray> mVertexArray;
      std::vector<Submesh> mSubmeshes;
      bool mIsWireframe{ false };

      //MATERIALS VECTOR TO ADD
      //AABB mBoundingBox add in the future
	};
}

