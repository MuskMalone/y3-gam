/*!*********************************************************************
\file   RegisterComponents.cpp
\date   15-September-2024
\brief  This file handles the registration of components used in
        our engine to be recognized by RTTR library. Anything that
        needs to be serialized/deserialized needs to be registered.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include <rttr/registration>
#include <Core/Component/Components.h>

RTTR_REGISTRATION
{
  using namespace Component;

  rttr::registration::class_<Tag>("Tag")
    .constructor<>()
    .property("tag", &Tag::tag);

  rttr::registration::class_<Transform>("Transform")
    .constructor<>()
    .property("localPos", &Transform::localPos)
    .property("localScale", &Transform::localScale)
    .property("localRot", &Transform::localRot)
    .property("worldPos", &Transform::worldPos)
    .property("worldScale", &Transform::worldScale)
    .property("worldRot", &Transform::worldRot);

  rttr::registration::class_<Layer>("Layer")
    .constructor<>()
    .property("layerName", &Layer::name);

  rttr::registration::class_<Mesh>("Mesh")
    .constructor<>()
    .property("meshRef", &Mesh::meshRef);

  rttr::registration::class_<RigidBody>("RigidBody")
    .constructor<>()
    .property("velocity", &RigidBody::velocity)
    .property("angularVelocity", &RigidBody::angularVelocity)
    .property("friction", &RigidBody::friction)
    .property("restitution", &RigidBody::restitution)
    .property("gravityFactor", &RigidBody::gravityFactor)
    .property("motionType", &RigidBody::motionType);

  rttr::registration::class_<Collider>("Collider")
    .constructor<>()
    .property("scale", &Collider::scale)
    .property("positionOffset", &Collider::positionOffset)
    .property("rotationOffset", &Collider::rotationOffset)
    .property("type", &Collider::type);

  rttr::registration::class_<PrefabOverrides>("PrefabOverrides")
    .constructor<>()
    .property("prefabName", &PrefabOverrides::prefabName)
    .property("propOverrides", &PrefabOverrides::propOverrides)
    .property("addedComponents", &PrefabOverrides::addedComponents)
    .property("removedComponents", &PrefabOverrides::removedComponents);
}
