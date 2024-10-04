#include <pch.h>
#include <Scripting/ScriptingSystem.h>
#include <Core/Components/Script.h>

using namespace Mono;

void ScriptingSystem::Update()
{
  for (ECS::Entity entity : mEntityManager.GetAllEntitiesWithComponents<Component::Script>()) {
    Component::Script& scriptComp = entity.GetComponent<Component::Script>();
    scriptComp.UpdateAllScripts();
  }
}
