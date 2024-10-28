#pragma once
/*!*********************************************************************
\file   Script.h
\author han.q\@digipen.edu
\date   5 October 2024
\brief
	This file contains the declaration of Script Component struct. This component
	has a vector that stores all the c# script attached to the entity

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <string>
#include <glm/glm.hpp>

namespace Component {
	
	enum LightType {
			DIRECTIONAL,
			SPOTLIGHT,
			LIGHT_COUNT
		};

	// @TODO: for use when shadows are improved on
	struct ShadowConfig {
		float bias;
		// softness / shadow type
	};

	struct Light {
		Light() = default;

		inline void Clear() noexcept { 
			type = DIRECTIONAL;
			position = direction = color = {};
			// idk what the default values are

			castShadows = false;
			bias = 0.05f;
			nearPlane = 0.2f;
		}

		glm::vec3 position;        // Position of the light (used for Point and Spotlight)
		glm::vec3 direction;       // Direction of the light (used for Directional and Spotlight)
		glm::vec3 color;           // Color of the light
		//ShadowConfig shadowConfig;
		LightType type;
		float linear;
		float quadratic;
		float radius;

		float cutOff;				// Spotlight
		float outerCutOff;  // Spotlight
		
		float bias = 0.05f;				// Shadows
		float nearPlane = 0.2f;		// Shadows
		bool castShadows = false;	// Shadows
	};

} // namespace Component