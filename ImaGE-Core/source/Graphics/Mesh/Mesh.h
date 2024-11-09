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
#include "Asset/SmartPointer.h"

namespace Graphics {
	class Mesh {
    public:
      Mesh() {};
      Mesh(IGE::Assets::GUID meshSource ): mMeshSource{ meshSource }{}

      const IGE::Assets::GUID& GetMeshSource() const { return mMeshSource; }
      void SetMeshSource(const IGE::Assets::GUID& meshSrc) { mMeshSource = meshSrc;}
      //const std::vector<std::shared_ptr<Material>>& GetMaterials() const { return mMaterials; }

    private:
      IGE::Assets::GUID mMeshSource;  // The blueprint (geometry and submeshes)
      //std::vector<uint32_t> m_Submeshes;  // Indices to submeshes in MeshSource - Maybe need in future?
      // std::vector<std::shared_ptr<Material>> mMaterials;  // Instance-specific mat @TODO CHANGE TO MATERIAL TABLE INSTEAD?
	};
}
