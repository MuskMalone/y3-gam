#pragma once
#include "Layer.h"
#include "Script.h"
#include "Tag.h"
#include "Text.h"
#include "Transform.h"
#include "Collider.h"
#include "RigidBody.h"
#include "Material.h"
#include "Mesh.h"

// @TODO: EDIT WHEN NEW COMPONENTS
namespace Component {
  static std::list<std::string> ComponentNameList{
    "Layer",
    "Script",
    "Tag",
    "Text",
    "Transform"
  };
}