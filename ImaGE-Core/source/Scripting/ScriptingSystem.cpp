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

using namespace Mono;

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
