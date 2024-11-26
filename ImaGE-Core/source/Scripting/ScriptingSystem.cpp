/*!*********************************************************************
\file   ScriptSystem.h
\author han.q\@digipen.edu
\date   5 October 2024
\brief
  This file contains the declaration of ScriptSystem Class. This system is in
  charge of calling all the entities' scripts' update function

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include <Scripting/ScriptingSystem.h>
#include <Core/Components/Script.h>
#include <Events/EventManager.h>
using namespace Mono;

Mono::ScriptingSystem::ScriptingSystem(const char* name) : System(name) {
    SUBSCRIBE_CLASS_FUNC(Events::EventType::ENTITY_MOUSE_ENTER, &ScriptingSystem::HandleMouseEnter, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::ENTITY_MOUSE_EXIT, &ScriptingSystem::HandleMouseExit, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::ENTITY_MOUSE_DOWN, &ScriptingSystem::HandleMouseDown, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::ENTITY_MOUSE_UP, &ScriptingSystem::HandleMouseUp, this);
}

void ScriptingSystem::Update()
{
  if (Mono::ScriptManager::GetInstance().mTriggerStart)
  {
    Mono::ScriptManager::GetInstance().mTriggerStart = false;
    for (ECS::Entity entity : mEntityManager.GetAllEntitiesWithComponents<Component::Script>()) {
      Component::Script& scriptComp = entity.GetComponent<Component::Script>();
      scriptComp.TriggerScriptsStart();
    }

  }
  for (ECS::Entity entity : mEntityManager.GetAllEntitiesWithComponents<Component::Script>()) {
    Component::Script& scriptComp = entity.GetComponent<Component::Script>();
    scriptComp.UpdateAllScripts();
  }
}

EVENT_CALLBACK_DEF(Mono::ScriptingSystem, HandleMouseEnter) {
	auto e{ CAST_TO_EVENT(Events::EntityMouseEnter) };
    if (e->mEntity.HasComponent<Component::Script>()) {
        auto& scriptList{ e->mEntity.GetComponent<Component::Script>().mScriptList };
        for (Mono::ScriptInstance& cs : scriptList)
        {
            if (scriptList.size() == 0) //This is in case a script is deleted while being updated
                break;
            cs.InvokeOnMouseEnter();
        }
    }
}

EVENT_CALLBACK_DEF(Mono::ScriptingSystem, HandleMouseExit) {
	auto e{ CAST_TO_EVENT(Events::EntityMouseExit) };
    if (e->mEntity.HasComponent<Component::Script>()) {
        auto& scriptList{ e->mEntity.GetComponent<Component::Script>().mScriptList };
        for (Mono::ScriptInstance& cs : scriptList)
        {
            if (scriptList.size() == 0) //This is in case a script is deleted while being updated
                break;
            cs.InvokeOnMouseExit();
        }
    }
}

EVENT_CALLBACK_DEF(Mono::ScriptingSystem, HandleMouseDown) {
    auto e{ CAST_TO_EVENT(Events::EntityMouseDown) };
    if (e->mEntity.HasComponent<Component::Script>()) {
        auto& scriptList{ e->mEntity.GetComponent<Component::Script>().mScriptList };
        for (Mono::ScriptInstance& cs : scriptList)
        {
            if (scriptList.size() == 0) //This is in case a script is deleted while being updated
                break;
            cs.InvokeOnMouseDown();
        }
    }
}

EVENT_CALLBACK_DEF(Mono::ScriptingSystem, HandleMouseUp) {
    auto e{ CAST_TO_EVENT(Events::EntityMouseUp) };
    if (e->mEntity.HasComponent<Component::Script>()) {
        auto& scriptList{ e->mEntity.GetComponent<Component::Script>().mScriptList };
        for (Mono::ScriptInstance& cs : scriptList)
        {
            if (scriptList.size() == 0) //This is in case a script is deleted while being updated
                break;
            cs.InvokeOnMouseUp();
        }
    }
}
