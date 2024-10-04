/*!*********************************************************************
\file   Mesh.h
\author 
\date   5-October-2024
\brief  Definition of the Mesh component, which denotes which model
				the entity is rendered with
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include "Graphics/Mesh.h"
#include <string>

namespace Component {
	struct Mesh{
		Mesh() : meshName{ "None" }, mesh{} {};
		Mesh(std::shared_ptr<Graphics::Mesh> meshSource, std::string name)
			: meshName{ std::move(name) }, mesh{ meshSource } {}

		inline void Clear() noexcept {
			mesh.reset();
		}

		std::string meshName;
		std::shared_ptr<Graphics::Mesh> mesh;
	};
} // namespace Component