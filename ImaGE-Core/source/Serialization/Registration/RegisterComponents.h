/*!*********************************************************************
\file   RegisterComponents.cpp
\date   15-September-2024
\brief  This file handles the registration of components used in
        our engine to be recognized by RTTR library. Anything that
        needs to be serialized/deserialized needs to be registered.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <rttr/policy.h>
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
  rttr::registration::class_<ClassType>(name).constructor<>()(rttr::policy::ctor::as_object).method("Clear", &ClassType::Clear)

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
    .property("tag", &Tag::tag)
    .property("isActive", &Tag::isActive);

  REGISTER_COMPONENT(Transform, "Transform")
    .property("position", &Transform::position)
    .property("scale", &Transform::scale)
    .property("rotation", &Transform::rotation)
    .property("eulerAngles", &Transform::eulerAngles)
    .property("worldPos", &Transform::worldPos)
    .property("worldScale", &Transform::worldScale)
    .property("worldRot", &Transform::worldRot);

  REGISTER_COMPONENT(Light, "Light")
    .property("lightType", &Light::type)
    .property("color", &Light::color)
    .property("intensity", &Light::mLightIntensity)
    .property("innerAngle", &Light::mInnerSpotAngle)
    .property("outerAngle", &Light::mOuterSpotAngle)
    .property("range", &Light::mRange)
    .property("castShadows", &Light::castShadows)
    .property("shadowConfig", &Light::shadowConfig);

  REGISTER_COMPONENT(Layer, "Layer")
    .property("layerName", &Layer::name);

  REGISTER_COMPONENT(Mesh, "Mesh")
    .property("meshName", &Mesh::meshName)
    .property("GUID", &Mesh::meshSource)
    .property("submeshIdx", &Mesh::submeshIdx)
    .property("isCustomMesh", &Mesh::isCustomMesh)
    .property("castShadows", &Mesh::castShadows)
    .property("receiveShadows", &Mesh::receiveShadows);

  REGISTER_COMPONENT(Material, "Material")
    .property("materialGUID", &Material::materialGUID);

  REGISTER_COMPONENT(Text, "Text")
    .property("textContent", &Text::textContent)
    .property("color", &Text::color)
    .property("scale", &Text::scale)
    .property("textAssetGUID", &Text::textAsset)
    .property("fontFamilyName", &Text::fontFamilyName)
    .property("alignment", &Text::alignment)
    .property("multiLineSpacingOffset", &Text::multiLineSpacingOffset)
    .property("textBoxEnabledFlag", &Text::textBoxEnabledFlag)
    .property("textBoxDimensions", &Text::textBoxDimensions);

  REGISTER_COMPONENT(RigidBody, "RigidBody")
    .property("velocity", &RigidBody::velocity)
    .property("angularVelocity", &RigidBody::angularVelocity)
    .property("staticFriction", &RigidBody::staticFriction)
    .property("dynamicFriction", &RigidBody::dynamicFriction)
    .property("restitution", &RigidBody::restitution)
    .property("gravityFactor", &RigidBody::gravityFactor)
    .property("linearDamping", &RigidBody::linearDamping)
    .property("motionType", &RigidBody::motionType)
    .property("axisLock", &RigidBody::axisLock)
    .property("angularAxisLock", &RigidBody::angularAxisLock)
    .property("hasJoint", &RigidBody::hasJoint)
    .property("entityLinked", &RigidBody::entityLinked)
    .property("joinConfig", &RigidBody::jointConfig)
    .property("jointOffset", &RigidBody::jointOffset)
    .property("entityLinkedJointOffset", &RigidBody::entityLinkedJointOffset);

  REGISTER_COMPONENT(BoxCollider, "BoxCollider")
    .property("scale", &BoxCollider::scale)
    .property("positionOffset", &BoxCollider::positionOffset)
    .property("degreeRotationOffsetEuler", &BoxCollider::degreeRotationOffsetEuler)
    .property("rotationOffset", &BoxCollider::rotationOffset)
    .property("sensor", &BoxCollider::sensor);

  REGISTER_COMPONENT(SphereCollider, "SphereCollider")
    .property("radius", &SphereCollider::radius)
    .property("positionOffset", &SphereCollider::positionOffset)
    .property("degreeRotationOffsetEuler", &SphereCollider::degreeRotationOffsetEuler)
    .property("rotationOffset", &SphereCollider::rotationOffset)
    .property("sensor", &SphereCollider::sensor);

  REGISTER_COMPONENT(CapsuleCollider, "CapsuleCollider")
    .property("radius", &CapsuleCollider::radius)
    .property("halfheight", &CapsuleCollider::halfheight)
    .property("positionOffset", &CapsuleCollider::positionOffset)
    .property("degreeRotationOffsetEuler", &CapsuleCollider::degreeRotationOffsetEuler)
    .property("rotationOffset", &CapsuleCollider::rotationOffset)
    .property("sensor", &CapsuleCollider::sensor);

  REGISTER_COMPONENT(Script, "ScriptComponent")
    .constructor<std::vector<std::string> const&>()
    .property(JSON_SCRIPT_LIST_KEY, &Script::mScriptList);

  REGISTER_COMPONENT(AudioSource, "AudioSource")
    .property("sounds", &AudioSource::sounds);
  REGISTER_COMPONENT(AudioListener, "AudioListener");

  REGISTER_COMPONENT(Bloom, "Bloom")
    .property("threshold", &Bloom::threshold)
    .property("intensity", &Bloom::intensity)
    .property("range", &Bloom::range);
  REGISTER_COMPONENT(Canvas, "Canvas")
    .property("isVisible", &Component::Canvas::isVisible)
    .property("hasTransition", &Component::Canvas::hasTransition)
    .property("transitionProgress", &Component::Canvas::transitionProgress)
    .property("transitionSpeed", &Component::Canvas::transitionSpeed)
    .property("fadeColor", &Component::Canvas::fadeColor)
    .property("fadingOut", &Component::Canvas::fadingOut);

  REGISTER_COMPONENT(Image, "Image")
    .property("color", &Image::color)
    .property("textureAssetGUID", &Image::textureAsset);

  REGISTER_COMPONENT(Sprite2D, "Sprite2D")
    .property("color", &Sprite2D::color)
    .property("textureAssetGUID", &Sprite2D::textureAsset)
    .property("isTransparent", &Sprite2D::isTransparent);

  REGISTER_COMPONENT(Animation, "Animation")
    .property("animations", &Animation::animations)
    .property("currentAnimation", &Animation::currentAnimation)
    .property("repeat", &Animation::repeat);

  REGISTER_COMPONENT(Camera, "Camera")
    .property("projType", &Component::Camera::projType)
    .property("position", &Component::Camera::position)
    .property("yaw", &Component::Camera::yaw)
    .property("pitch", &Component::Camera::pitch)
    .property("fov", &Component::Camera::fov)
    .property("aspectRatio", &Component::Camera::aspectRatio)
    .property("nearClip", &Component::Camera::nearClip)
    .property("farClip", &Component::Camera::farClip);

  REGISTER_COMPONENT(Skybox, "Skybox")
    //.property("materialAssetGUID", &Skybox::materialAsset);
    .property("textureAssetGUID1", &Skybox::tex1)
    .property("textureAssetGUID2", &Skybox::tex2)
    .property("blend", &Skybox::blend);

  REGISTER_COMPONENT(Interactive, "Interactive")
    .property("isHovered", &Interactive::isHovered);

  REGISTER_COMPONENT(EmitterSystem, "EmitterSystem")
    .property("emitters", &EmitterSystem::emitters);

  REGISTER_COMPONENT(Video, "Video")
    .property("renderType", &Video::renderType)
    .property("guid", &Video::guid)
    .property("audioEnabled", &Video::audioEnabled)
    .property("playOnStart", &Video::playOnStart);

    // stuff below are not actual "Components", hence we skip the REGISTER_COMPONENT checks
  rttr::registration::class_<PrefabOverrides>("PrefabOverrides")
    .constructor<>()(rttr::policy::ctor::as_object)
    .property(JSON_GUID_KEY, &PrefabOverrides::guid)
    .property("modifiedComponents", &PrefabOverrides::modifiedComponents)
    .property("removedComponents", &PrefabOverrides::removedComponents)
    .property("subDataId", &PrefabOverrides::subDataId);

  rttr::registration::class_<Reflection::ProxyScriptComponent>("ProxyScriptComponent")
    .constructor<>()(rttr::policy::ctor::as_object)
    .property(JSON_SCRIPT_LIST_KEY, &Reflection::ProxyScriptComponent::proxyScriptList);
}
