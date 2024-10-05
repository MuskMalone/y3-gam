/*!*********************************************************************
\file   Mesh.h
\author k.choa\@digipen.edu
\date   22/09/2024
\brief  The Mesh class encapsulates a 3D mesh, storing a reference to the underlying geometry
        and submeshes via a MeshSource. It provides functionality to get and set the associated
        MeshSource, which acts as the blueprint for the mesh.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include "MeshSource.h"

namespace Graphics {
	class Mesh {
    public:
      Mesh() {};
      Mesh(std::shared_ptr<MeshSource> meshSource ): mMeshSource{ std::move(meshSource) }{}

      const std::shared_ptr<MeshSource>& GetMeshSource() const { return mMeshSource; }
      void SetMeshSource(const std::shared_ptr<MeshSource>& meshSrc) { mMeshSource = meshSrc;}
      //const std::vector<std::shared_ptr<Material>>& GetMaterials() const { return mMaterials; }

    private:
      std::shared_ptr<MeshSource> mMeshSource;  // The blueprint (geometry and submeshes)
      //std::vector<uint32_t> m_Submeshes;  // Indices to submeshes in MeshSource - Maybe need in future?
      // std::vector<std::shared_ptr<Material>> mMaterials;  // Instance-specific mat @TODO CHANGE TO MATERIAL TABLE INSTEAD?
	};
}
