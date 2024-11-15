#pragma once
/*!*********************************************************************
\file   Light.h
\author han.q\@digipen.edu
\date   27 October 2024
\brief
	This file contains the declaration of Light Component struct. It
	contains the data members necessary to cast a light on the scene
	and shadows if necessary.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <glm/glm.hpp>

namespace Component {
	
	enum LightType {
			DIRECTIONAL,
			SPOTLIGHT,
			LIGHT_COUNT
		};

	// @TODO: for organization of shadow members
	struct ShadowConfig {
		ShadowConfig() : centerPos{}, softness { 1 }, bias{ 0.005f }, nearPlane{ -100.f },
			isStatic{ true }, shadowModified{ true }, customCenter{ false } {}

		glm::vec3 centerPos;
		int softness;
		float bias;
		float nearPlane;
		bool isStatic;
		bool shadowModified, customCenter;	// im using this flag to determine when to recompute the light space mtx

		void Clear() {
			centerPos = {};
			isStatic = shadowModified = true;
			customCenter = false;
			bias = 0.005f;
			softness = 1;
			nearPlane = -100.f;
		}
	};

	struct Light {
		Light() : forwardVec{ 0.f, 0.f, -1.f }, color{ 1.f, 1.f, 1.f }, type{ DIRECTIONAL }, mLightIntensity{ 1.f },
			mInnerSpotAngle{ 5.f }, mOuterSpotAngle{ 12.5f }, mRange{ 10.f }, shadowConfig{}, castShadows{ false } {}

		inline void Clear() noexcept {
			forwardVec = { 0.f, 0.f, -1.f };
			type = DIRECTIONAL;
			color = { 1.f, 1.f, 1.f };
			mLightIntensity = 1.f;

			mInnerSpotAngle = 5.f;
			mOuterSpotAngle = 12.5f;
			mRange = 10.f;
			
			shadowConfig.Clear();
			castShadows = false;
		}

		glm::vec3 forwardVec;		// Set default forward vector to face the -z-axis
		glm::vec3 color;				// Color of the light
		LightType type;
		float mLightIntensity;	// Intensity of the light

		// SpotLight 
		float mInnerSpotAngle;  // Inner spot angle in degrees
		float mOuterSpotAngle;	// Outer spot angle in degrees
		float mRange;						// Maximum range of the spotlight
		
		// Shadows
		ShadowConfig shadowConfig;

		bool castShadows;
	};

} // namespace Component