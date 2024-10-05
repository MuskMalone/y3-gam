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
		using MeshSourcePtr = std::shared_ptr<MeshSource>;

		// using this to get model based on string in Mesh component
		// Options: Cube, Plane
		static MeshSourcePtr CreateModelFromString(std::string const& meshName);
		static MeshSourcePtr CreateModelFromImport(std::string const& file);	// load from file (obj, fbx etc.)

	private:
		static MeshSourcePtr CreateCube();
		static MeshSourcePtr CreatePlane();
		static MeshSourcePtr CreatePyramid();
	};
}
