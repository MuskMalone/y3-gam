#pragma once
#include "Layer.h"
#include "Tag.h"
#include "Transform.h"
#include "Collider.h"
#include "RigidBody.h"
#include "Material.h"
#include "Mesh.h"

namespace Component {
  static std::list<std::string> ComponentNameList{
    "Layer",
    "Tag",
    "Transform"
  };
}