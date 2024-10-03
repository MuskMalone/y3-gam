#pragma once
#include "Graphics/Mesh.h"

//im just relocating struct Object's members to conform to the ECS
namespace Component {
	struct Mesh{
		Mesh() {}; //to be removed
		Mesh(std::shared_ptr<Graphics::Mesh> meshSource)
			: mesh(meshSource) {}

		inline void Clear() noexcept {
			mesh.reset();
		}

		std::shared_ptr<Graphics::Mesh> mesh;
	};
} // namespace Component