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
#include <Events/EventManager.h>

namespace CMD {
	void TransformCmd(ParamPack const& pp)
	{
		ECS::Entity e{ std::any_cast<ECS::Entity>(pp[0]) };
		Component::Transform t{ std::any_cast<Component::Transform>(pp[1]) };
		(e.GetComponent<Component::Transform>() = t).modified = true;	// set modified flag
	}


	CommandManager::CommandManager() {
		SUBSCRIBE_CLASS_FUNC(Events::SceneStateChange, &CommandManager::OnSceneChange, this);
	}

	// clear command stack when scene changes
	EVENT_CALLBACK_DEF(CommandManager, OnSceneChange) {
		switch (CAST_TO_EVENT(Events::SceneStateChange)->mNewState) {
		case Events::SceneStateChange::NEW:
		case Events::SceneStateChange::CHANGED:
			mCommandStack.clear();
			break;
		default:break;
		}
	}

}	//namespace CMD
