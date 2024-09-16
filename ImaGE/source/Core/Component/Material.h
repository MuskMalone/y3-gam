#pragma once
#include <pch.h>

//im just relocating struct Object's members to conform to the ECS
namespace Component {
	struct Material {
		glm::vec4 clr;
		//textures etc
	};
}