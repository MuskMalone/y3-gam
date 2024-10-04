/*!*********************************************************************
\file   RegisterComponents.cpp
\date   15-September-2024
\brief  This file handles the registration of components used in
        our engine to be recognized by RTTR library. Anything that
        needs to be serialized/deserialized needs to be registered.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <rttr/registration>
#include <Core/Components/Components.h>

static void rttr_auto_register_reflection_function2_(); namespace {
  struct rttr__auto__register2__ {
    rttr__auto__register2__() {
      rttr_auto_register_reflection_function2_();
    }
  };
} static const rttr__auto__register2__ auto_register__20; static void rttr_auto_register_reflection_function2_()
{
  using namespace Component;

  rttr::registration::class_<Tag>("Tag")
    .constructor<>()
    .property("tag", &Tag::tag);

  rttr::registration::class_<Transform>("Transform")
    .constructor<>()
    .property("position", &Transform::position)
    .property("scale", &Transform::scale)
    .property("rotation", &Transform::rotation)
    .property("eulerAngles", &Transform::eulerAngles)
    .property("worldPos", &Transform::worldPos)
    .property("worldScale", &Transform::worldScale)
    .property("worldRot", &Transform::worldRot);

  rttr::registration::class_<Layer>("Layer")
    .constructor<>()
    .property("layerName", &Layer::name);

  rttr::registration::class_<Mesh>("Mesh")
    .constructor<>()
    .property("meshName", &Mesh::meshName);

  rttr::registration::class_<Material>("Material")
    .constructor<>()
    ;
    //.property("color", &Material::color);

  rttr::registration::class_<Text>("Text")
    .constructor<>()
    .property("textContent", &Text::textContent)
    .property("fontName", &Text::fontName)
    .property("color", &Text::color)
    .property("scale", &Text::scale);

  rttr::registration::class_<RigidBody>("RigidBody")
    .constructor<>()
    .property("velocity", &RigidBody::velocity)
    .property("angularVelocity", &RigidBody::angularVelocity)
    .property("friction", &RigidBody::staticFriction)
    .property("restitution", &RigidBody::restitution)
    .property("gravityFactor", &RigidBody::gravityFactor)
    .property("motionType", &RigidBody::motionType);

  rttr::registration::class_<Collider>("Collider")
      .constructor<>()
      .property("scale", &Collider::scale)
      .property("positionOffset", &Collider::positionOffset);
    //.property("rotationOffset", &Collider::rotationOffset)
    //.property("type", &Collider::type);


  rttr::registration::class_<PrefabOverrides>("PrefabOverrides")
    .constructor<>()
    .property("prefabName", &PrefabOverrides::prefabName)
    .property("modifiedComponents", &PrefabOverrides::modifiedComponents)
    .property("removedComponents", &PrefabOverrides::removedComponents)
    .property("subDataId", &PrefabOverrides::subDataId);
}
