#pragma once
#include "MeshSource.h"
#include "Asset/SmartPointer.h"
namespace Graphics {
	class Mesh {
    public:
      Mesh() {};
      Mesh(IGE::Assets::GUID meshSource): mMeshSource{ std::move(meshSource) }{}

      const IGE::Assets::GUID& GetMeshSource() const { return mMeshSource; }
      void SetMeshSource(const IGE::Assets::GUID& meshSrc) { mMeshSource = meshSrc;}
      //const std::vector<std::shared_ptr<Material>>& GetMaterials() const { return mMaterials; }

    private:
      //std::shared_ptr<MeshSource> 
      IGE::Assets::GUID mMeshSource;  // The blueprint (geometry and submeshes)
      //std::vector<uint32_t> m_Submeshes;  // Indices to submeshes in MeshSource - Maybe need in future?
      // std::vector<std::shared_ptr<Material>> mMaterials;  // Instance-specific mat @TODO CHANGE TO MATERIAL TABLE INSTEAD?
	};
}
