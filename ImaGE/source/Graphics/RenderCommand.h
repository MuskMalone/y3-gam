#pragma once
#include <pch.h>
#include "Mesh.h"

namespace Graphics {
	struct RenderCommand {
		std::shared_ptr<Mesh> mesh;
		glm::mat4 transform;
		//material
		//color?
	};
}