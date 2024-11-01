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
		rttr::type::get<Component::Script>()

	};

} // namespace Reflection
