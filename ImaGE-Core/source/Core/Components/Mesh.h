/*!*********************************************************************
\file   Mesh.h
\author 
\date   5-October-2024
\brief  Definition of the Mesh component, which denotes which model
				the entity is rendered with
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <string>
#include <Asset/IGEAssets.h>

namespace Component {
	struct Mesh{
		Mesh() : meshName{ "None" }, meshSource{}, isCustomMesh{ false }, castShadows{ true }, receiveShadows{ true } {}
		Mesh(IGE::Assets::GUID const& meshSrc, std::string name, bool custom = false) :
			meshName{ std::move(name) }, meshSource { meshSrc }, isCustomMesh{ custom }, castShadows{ true }, receiveShadows{ true } {}

		const IGE::Assets::GUID& GetMeshSource() const { return meshSource; }
		void SetMeshSource(const IGE::Assets::GUID& meshSrc) { meshSource = meshSrc; }
		//const std::vector<std::shared_ptr<Material>>& GetMaterials() const { return mMaterials; }

		inline void Clear() noexcept {
			meshName = "None";
			meshSource = {};
			castShadows = receiveShadows = true;
			isCustomMesh = false;
		}

		std::string meshName;
		IGE::Assets::GUID meshSource;  // The blueprint (geometry and submeshes)
		bool isCustomMesh;
		bool castShadows;
		bool receiveShadows;	// not implemented yet
		//std::vector<uint32_t> m_Submeshes;  // Indices to submeshes in MeshSource - Maybe need in future?
		// std::vector<std::shared_ptr<Material>> mMaterials;  // Instance-specific mat @TODO CHANGE TO MATERIAL TABLE INSTEAD?
	};
} // namespace Component