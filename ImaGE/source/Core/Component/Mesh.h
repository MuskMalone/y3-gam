#pragma once
#include <pch.h>
#include <Graphics/Model.h>

namespace Component {
	struct Mesh {
		

		inline void Clear() noexcept { meshRef.reset(); }

		std::shared_ptr<Graphics::Model> meshRef;
	};
} // namespace Component