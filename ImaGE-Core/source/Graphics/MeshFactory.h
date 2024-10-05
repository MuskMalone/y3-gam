#pragma once
#include "MeshSource.h"

namespace Graphics{
	class MeshFactory{
	public:
		//using MeshSourcePtr = MeshSource;//std::shared_ptr<MeshSource>;

		// using this to get model based on string in Mesh component
		// Options: Cube, Plane
		static MeshSource CreateModelFromString(std::string const& meshName);
		static MeshSource CreateModelFromImport(std::string const& file);	// load from file (obj, fbx etc.)

	private:
		static MeshSource CreateCube();
		static MeshSource CreatePlane();
		static MeshSource CreatePyramid();
	};
}
