#pragma once
#include "MeshSource.h"

namespace Graphics{
	class MeshFactory{
	public:
		using MeshSourcePtr = std::shared_ptr<MeshSource>;

		// Static method to create a cube mesh
		static MeshSourcePtr CreateCube();
		static MeshSourcePtr CreatePyramid();
#ifndef IMGUI_DISABLE
		static MeshSourcePtr CreateModelFromImport();	// load from file (obj, fbx etc.)
#endif
	};
}
