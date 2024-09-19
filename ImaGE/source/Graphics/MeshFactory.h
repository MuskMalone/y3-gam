#pragma once
#include "MeshSource.h"

namespace Graphics{
	class MeshFactory{
	public:
		// Static method to create a cube mesh
		static std::shared_ptr<MeshSource> CreateCube();
	};
}