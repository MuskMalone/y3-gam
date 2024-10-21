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
#include <Reflection/ProxyScript.h>
#include <Serialization/JsonKeys.h>
#include <Reflection/Metadata.h>

// don't mind me, just some templates to allow compile-time checks
namespace AssertStuff {
  template <typename, typename = void>
  constexpr bool HasClear = false;

  template<typename T>
  constexpr bool HasClear<T, std::void_t<decltype(std::declval<T>().Clear())>> = true;
}

// macro to ensure ur component defines some basic stuff
#define REGISTER_COMPONENT(ClassType, name) \
  static_assert(std::is_default_constructible<ClassType>::value, "Component " name " must be default-constructible!");\
  static_assert(AssertStuff::HasClear<ClassType>, "Component " name " needs to define a function Clear()!!"); \
  static_assert(std::is_same<decltype(&ClassType::Clear), void(ClassType::*)() noexcept>::value, \
    "Clear function of Component " name " does not have signature of \"void Clear() noexcept\"");\
  rttr::registration::class_<ClassType>(name).constructor<>().method("Clear", &ClassType::Clear)

static void rttr_auto_register_reflection_function2_(); namespace {
  struct rttr__auto__register2__ {
    rttr__auto__register2__() {
      rttr_auto_register_reflection_function2_();
    }
  };
} static const rttr__auto__register2__ auto_register__20; static void rttr_auto_register_reflection_function2_()
{
  using namespace Component;

  REGISTER_COMPONENT(Tag, "Tag")
    .property("tag", &Tag::tag);

  REGISTER_COMPONENT(Transform, "Transform")
    .property("position", &Transform::position)
    .property("scale", &Transform::scale)
    .property("rotation", &Transform::rotation)
    .property("eulerAngles", &Transform::eulerAngles)
    .property("worldPos", &Transform::worldPos)(
        rttr::metadata(Reflection::InspectorMetadata::DISABLED, true)
      )
    .property("worldScale", &Transform::worldScale)(
      rttr::metadata(Reflection::InspectorMetadata::DISABLED, true)
      )
    .property("worldRot", &Transform::worldRot)(
      rttr::metadata(Reflection::InspectorMetadata::DISABLED, true)
      );

  REGISTER_COMPONENT(Layer, "Layer")
    .property("layerName", &Layer::name);

  REGISTER_COMPONENT(Mesh, "Mesh")
    .property("meshName", &Mesh::meshName);

  REGISTER_COMPONENT(Material, "Material")
    .property("matIdx", &Material::matIdx);

  REGISTER_COMPONENT(Text, "Text")
    .property("textContent", &Text::textContent)
    .property("fontName", &Text::fontName)
    .property("color", &Text::color)
    .property("scale", &Text::scale);

  REGISTER_COMPONENT(RigidBody, "RigidBody")
    .property("velocity", &RigidBody::velocity)
    .property("angularVelocity", &RigidBody::angularVelocity)
    .property("staticFriction", &RigidBody::staticFriction)
    .property("dynamicFriction", &RigidBody::dynamicFriction)
    .property("restitution", &RigidBody::restitution)
    .property("gravityFactor", &RigidBody::gravityFactor)
    .property("linearDamping", &RigidBody::linearDamping)
    .property("motionType", &RigidBody::motionType);

  REGISTER_COMPONENT(BoxCollider, "BoxCollider")
      .property("scale", &BoxCollider::scale)
      .property("positionOffset", &BoxCollider::positionOffset)
      .property("rotationOffset", &BoxCollider::rotationOffset)
      .property("sensor", &BoxCollider::sensor);

  REGISTER_COMPONENT(SphereCollider, "SphereCollider")
      .property("radius", &SphereCollider::radius)
      .property("positionOffset", &SphereCollider::positionOffset)
      .property("rotationOffset", &SphereCollider::rotationOffset)
      .property("sensor", &SphereCollider::sensor);

  REGISTER_COMPONENT(CapsuleCollider, "CapsuleCollider")
      .property("radius", &CapsuleCollider::radius)
      .property("halfheight", &CapsuleCollider::halfheight)
      .property("positionOffset", &CapsuleCollider::positionOffset)
      .property("rotationOffset", &CapsuleCollider::rotationOffset)
      .property("sensor", &CapsuleCollider::sensor);

  REGISTER_COMPONENT(Script, "ScriptComponent")
    .constructor<std::vector<std::string> const&>()
    .property(JSON_SCRIPT_LIST_KEY, &Script::mScriptList);



  // stuff below are not actual "Components", hence we skip the REGISTER_COMPONENT checks
  rttr::registration::class_<PrefabOverrides>("PrefabOverrides")
    .constructor<>()
    .property("prefabName", &PrefabOverrides::prefabName)
    .property("modifiedComponents", &PrefabOverrides::modifiedComponents)
    .property("removedComponents", &PrefabOverrides::removedComponents)
    .property("subDataId", &PrefabOverrides::subDataId);

  rttr::registration::class_<Reflection::ProxyScriptComponent>("ProxyScriptComponent")
    .constructor<>()
    .property(JSON_SCRIPT_LIST_KEY, &Reflection::ProxyScriptComponent::proxyScriptList);
}
