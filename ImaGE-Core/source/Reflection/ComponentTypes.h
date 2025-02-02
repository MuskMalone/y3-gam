/*!*********************************************************************
\file   ComponentTypes.h
\date   15-September-2024
\brief	Contains the list of component types used for easy access
				during reflection.
	
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <vector>
#include <rttr/type.h>
#include <Core/Components/Components.h>
#include <typeindex>
#include <bitset>

namespace Reflection
{
	std::vector<rttr::type> const gComponentTypes
	{
		rttr::type::get<Component::AudioListener>(),
		rttr::type::get<Component::AudioSource>(),
		rttr::type::get<Component::Bloom>(),
		rttr::type::get<Component::Tag>(),
		rttr::type::get<Component::Transform>(),
		rttr::type::get<Component::Layer>(),
		rttr::type::get<Component::Material>(),
		rttr::type::get<Component::Mesh>(),
		rttr::type::get<Component::BoxCollider>(),
		rttr::type::get<Component::SphereCollider>(),
		rttr::type::get<Component::CapsuleCollider>(),
		rttr::type::get<Component::RigidBody>(),
		rttr::type::get<Component::Text>(),
		rttr::type::get<Component::Light>(),
		rttr::type::get<Component::Script>(),
		rttr::type::get<Component::Canvas>(),
		rttr::type::get<Component::Image>(),
		rttr::type::get<Component::Sprite2D>(),
		rttr::type::get<Component::Animation>(),
		rttr::type::get<Component::Camera>(),
		rttr::type::get<Component::Skybox>(),
		rttr::type::get<Component::Interactive>(),
		rttr::type::get<Component::EmitterSystem>()
	};

} // namespace Reflection
