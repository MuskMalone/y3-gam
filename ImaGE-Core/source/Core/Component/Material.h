#pragma once
#include <pch.h>
#include "Color.h"

namespace Component {
	struct Material {
		Material() : color{ Color::COLOR_BLACK } {}
		Material(glm::vec4 color) : color{ color } {}

		inline void Clear() noexcept { color = Color::COLOR_BLACK; }

		glm::vec4 color;

		// @TODO: TEXTURES, ETC
	};
}