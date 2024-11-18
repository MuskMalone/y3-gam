/*!*********************************************************************
\file   MeshFactory.h
\author k.choa\@digipen.edu
\date   22/09/2024
\brief  The MeshFactory class is responsible for creating various 3D models, including 
		basic shapes such as cubes, planes, and pyramids. It also supports importing 3D 
		models from external files using a custom binary format (IMSH).

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include "MeshSource.h"

namespace Graphics{
	class MeshFactory{
	public:
		//using MeshSource = std::shared_ptr<MeshSource>;

		// using this to get model based on string in Mesh component
		// Options: Cube, Plane
		static MeshSource CreateModelFromString(std::string const& meshName);
		static MeshSource CreateModelFromImport(std::string const& file);	// load from file (obj, fbx etc.)

	private:
		static MeshSource CreateQuad();
		static MeshSource CreateCube();
		static MeshSource CreatePlane();
		static MeshSource CreateSphere(uint32_t stacks = 32, uint32_t slices = 32);
		static MeshSource CreateCapsule(float radius = 0.5f, float height = 1.0f, int stacks = 16, int slices = 16);
	};
}
