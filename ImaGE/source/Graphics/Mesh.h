#include <pch.h>
#include "MeshSource.h"

namespace Graphics {
	class Mesh {
    public:
        Mesh(std::shared_ptr<MeshSource> meshSource);

        const std::shared_ptr<MeshSource>& GetMeshSource() const { return mMeshSource; }
        void SetMeshSource(const std::shared_ptr<MeshSource>& meshSrc) { mMeshSource = meshSrc;}
        //const std::vector<std::shared_ptr<Material>>& GetMaterials() const { return mMaterials; }

    private:
        std::shared_ptr<MeshSource> mMeshSource;  // The blueprint (geometry and submeshes)
       //std::vector<uint32_t> m_Submeshes;  // Indices to submeshes in MeshSource - Maybe need in future?
       // std::vector<std::shared_ptr<Material>> mMaterials;  // Instance-specific mat @TODO CHANGE TO MATERIAL TABLE INSTEAD?
	};
}