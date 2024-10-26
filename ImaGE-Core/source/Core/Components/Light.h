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

	struct Light {
		
		Light() = default;



		inline void Clear() noexcept { }

		Light_Type mType;
		glm::vec3 mPosition;        // Position of the light (used for Point and Spotlight)
		glm::vec3 mDirection;       // Direction of the light (used for Directional and Spotlight)
		glm::vec3 mColor;           // Color of the light
		float mLinear;
		float mQuadratic;
		float mRadius;
		float mCutOff;       //Spotlight
		float mOuterCutOff;  //Spotlight
	

	};

} // namespace Component