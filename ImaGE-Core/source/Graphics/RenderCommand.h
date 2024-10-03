#pragma once
#include "Mesh.h"
#include <glm/glm.hpp>

namespace Graphics {
	struct RenderCommand {
		std::shared_ptr<Mesh> mesh;
		glm::mat4 transform;
		//material
		//color?
	};
}
