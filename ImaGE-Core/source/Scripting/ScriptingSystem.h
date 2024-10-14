/*!*********************************************************************
\file   ScriptSystem.h
\author han.q\@digipen.edu
\date   5 October 2024
\brief
	This file contains the declaration of ScriptSystem Class. This system is in 
	charge of calling all the entities' scripts' update function 

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <Core/Systems/System.h>

namespace Mono
{
	class ScriptingSystem : public Systems::System {
		
	public:

		ScriptingSystem(const char* name) : System(name)  {}
		void Update() override;

	};
}

