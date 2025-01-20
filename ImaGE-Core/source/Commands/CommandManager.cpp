/******************************************************************************/
/*!
\par        Image Engine
\file       CommandManager.cpp

\author     tan cheng hian (t.chenghian)
\date       Nov 10, 2023

\brief      CommandManager

\copyright  Copyright (C) 2023 DigiPen Institute of Technology. Reproduction
			or disclosure of this file or its contents without the prior
			written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include <pch.h>

#include "CommandManager.h"


void CMD::TransformCmd(ParamPack const& pp)
{
	ECS::Entity e{ std::any_cast<ECS::Entity>(pp[0]) };
	Component::Transform t{ std::any_cast<Component::Transform>(pp[1]) };
	e.GetComponent<Component::Transform>() = t;
}


CMD::CommandManager::CommandManager()
{
	mCommandStack.clear();
}