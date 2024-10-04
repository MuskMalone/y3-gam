#pragma once
#include "Graphics/Mesh.h"
#include <string>

namespace Component {
	struct Mesh{
		Mesh() : meshName{ "None" }, mesh{} {};
		Mesh(std::shared_ptr<Graphics::Mesh> meshSource, std::string name)
			: meshName{ std::move(name) }, mesh{ meshSource } {}

		inline void Clear() noexcept {
			mesh.reset();
		}

		std::string meshName;
		std::shared_ptr<Graphics::Mesh> mesh;
	};
} // namespace Component