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
	
	enum Light_Type {
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
			color = {};
			// idk what the default values are

			castShadows = false;
			bias = 0.05f;
		}

		Light_Type type;
		glm::vec3 forwardVec{ 0, 0, -1.f };       // Set default forward vector to face the -z-axis
		glm::vec3 color;						 // Color of the light
		float mLightIntensity{ 1.f }; // Intensity of the light

		//SpotLight 
		float mInnerSpotAngle{ 5.f };   // Inner spot angle in degrees
		float mOuterSpotAngle{ 12.5f }; // Outer spot angle in degrees
		float mRange{ 10.f }; // Maximum range of the spotlight
		
		float bias = 0.05f;				// Shadows
		bool castShadows = false;	// Shadows
	};

} // namespace Component