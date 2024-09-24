#pragma once
#include <pch.h>
#include <Graphics/Model.h>
#include "Graphics/Mesh.h"
//im just relocating struct Object's members to conform to the ECS
namespace Component {
	struct Mesh{
		Mesh() {}; //to be removed
		Mesh(std::shared_ptr<Graphics::Mesh> meshSource)
			: mesh(meshSource) {}

		inline void Clear() noexcept {
			meshRef.reset();
			mesh.reset();
		}

		std::shared_ptr<Graphics::Model> meshRef; //to be removed
		std::shared_ptr<Graphics::Mesh> mesh;
	};
} // namespace Component