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
