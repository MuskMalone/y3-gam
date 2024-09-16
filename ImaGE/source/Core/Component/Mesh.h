#pragma once
#include <pch.h>
#include <Graphics/Model.h>
//im just relocating struct Object's members to conform to the ECS
namespace Component {
	struct Mesh{
		std::shared_ptr<Graphics::Model> meshRef;
	};
}