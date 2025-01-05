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
    SUBSCRIBE_CLASS_FUNC(Events::EntityMouseEnter, &ScriptingSystem::HandleMouseEnter, this);
    SUBSCRIBE_CLASS_FUNC(Events::EntityMouseExit, &ScriptingSystem::HandleMouseExit, this);
    SUBSCRIBE_CLASS_FUNC(Events::EntityMouseDown, &ScriptingSystem::HandleMouseDown, this);
    SUBSCRIBE_CLASS_FUNC(Events::EntityMouseUp, &ScriptingSystem::HandleMouseUp, this);

    SUBSCRIBE_CLASS_FUNC(Events::EntityPointerEnter, &ScriptingSystem::HandlePointerEnter, this);
    SUBSCRIBE_CLASS_FUNC(Events::EntityPointerExit, &ScriptingSystem::HandlePointerExit, this);
    SUBSCRIBE_CLASS_FUNC(Events::EntityPointerDown, &ScriptingSystem::HandlePointerDown, this);
    SUBSCRIBE_CLASS_FUNC(Events::EntityPointerUp, &ScriptingSystem::HandlePointerUp, this);
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
      // std::cout << entity.GetTag() << ":SUPdate\n";
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

EVENT_CALLBACK_DEF(Mono::ScriptingSystem, HandlePointerEnter) {
    auto e{ CAST_TO_EVENT(Events::EntityPointerEnter) };
    if (e->mEntity.HasComponent<Component::Script>()) {
        auto& scriptList{ e->mEntity.GetComponent<Component::Script>().mScriptList };
        for (Mono::ScriptInstance& cs : scriptList)
        {
            if (scriptList.size() == 0) //This is in case a script is deleted while being updated
                break;
            cs.InvokeOnPointerEnter();
        }
    }
}

EVENT_CALLBACK_DEF(Mono::ScriptingSystem, HandlePointerExit) {
    auto e{ CAST_TO_EVENT(Events::EntityPointerExit) };
    if (e->mEntity.HasComponent<Component::Script>()) {
        auto& scriptList{ e->mEntity.GetComponent<Component::Script>().mScriptList };
        for (Mono::ScriptInstance& cs : scriptList)
        {
            if (scriptList.size() == 0) //This is in case a script is deleted while being updated
                break;
            cs.InvokeOnPointerExit();
        }
    }
}

EVENT_CALLBACK_DEF(Mono::ScriptingSystem, HandlePointerDown) {
    auto e{ CAST_TO_EVENT(Events::EntityPointerDown) };
    if (e->mEntity.HasComponent<Component::Script>()) {
        auto& scriptList{ e->mEntity.GetComponent<Component::Script>().mScriptList };
        for (Mono::ScriptInstance& cs : scriptList)
        {
            if (scriptList.size() == 0) //This is in case a script is deleted while being updated
                break;
            cs.InvokeOnPointerDown();
        }
    }
}

EVENT_CALLBACK_DEF(Mono::ScriptingSystem, HandlePointerUp) {
    auto e{ CAST_TO_EVENT(Events::EntityPointerUp) };
    if (e->mEntity.HasComponent<Component::Script>()) {
        auto& scriptList{ e->mEntity.GetComponent<Component::Script>().mScriptList };
        for (Mono::ScriptInstance& cs : scriptList)
        {
            if (scriptList.size() == 0) //This is in case a script is deleted while being updated
                break;
            cs.InvokeOnPointerUp();
        }
    }
}

