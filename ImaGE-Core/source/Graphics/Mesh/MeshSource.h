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
#include <BoundingVolumes/AABB.h>

// forward declarations
namespace ECS { class Entity; }
namespace IGE::Core { template <typename T> class GUID; }
namespace IGE::Assets { struct AssetGUIDTag; using GUID = IGE::Core::GUID<AssetGUIDTag>; }

namespace Graphics {
	class MeshSource {
    public:
      MeshSource(const std::shared_ptr<VertexArray>& vao, const std::vector<Submesh>& submeshes,
          const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
          : mVertices{ vertices }, mMeshNames{}, mIndices{ indices }, mVertexArray{ vao }, mSubmeshes{ submeshes }, mBoundingBox{} {}

      MeshSource(std::shared_ptr<VertexArray>&& vao,
        std::vector<Submesh>&& submeshes, std::vector<Vertex>&& vertices,
        std::vector<uint32_t>&& indices, std::vector<std::string>&& names)
        : mVertices{ std::move(vertices) }, mMeshNames{ names }, mIndices{ std::move(indices) },
        mVertexArray{ std::move(vao) }, mSubmeshes{ std::move(submeshes) }, mBoundingBox{} {}

      const std::vector<Vertex>& GetVertices() const { return mVertices; }
      const std::vector<uint32_t>& GetIndices() const { return mIndices; }

      const std::shared_ptr<VertexArray>& GetVertexArray() const { return mVertexArray; }
      const std::vector<Submesh>& GetSubmeshes() const { return mSubmeshes; }

      /*!*********************************************************************
      \brief
        Constructs an entity with the mesh source. All submeshes will be
        a child of an empty parent
      \param guid
        GUID of the mesh
      \param fileName
        The name of the file
      \return
        The root entity
      ************************************************************************/
      ECS::Entity ConstructEntity(IGE::Assets::GUID const& guid, std::string const& fileName) const;

      BV::AABB const& GetBoundingBox() const { return mBoundingBox; }

      void SetBoundingVolume(glm::vec3 const& min, glm::vec3 const& max);

      bool IsWireframe() { return mIsWireframe; }
      void ToggleWireframe() { mIsWireframe = !mIsWireframe; }
    private:
      std::vector<Vertex> mVertices;
      std::vector<std::string> mMeshNames;
      std::vector<uint32_t> mIndices;

      std::shared_ptr<VertexArray> mVertexArray;
      std::vector<Submesh> mSubmeshes;

      BV::AABB mBoundingBox;
      
      bool mIsWireframe{ false };

      //MATERIALS VECTOR TO ADD
	};
}

