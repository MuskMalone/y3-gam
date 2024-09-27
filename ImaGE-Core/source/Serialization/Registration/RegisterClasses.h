/*!*********************************************************************
\file   RegisterClasses.cpp
\date   15-September-2024
\brief  This file handles the registration of custom classes/structs
        used in our engine to be recognized by RTTR library. Anything
        that needs to be serialized/deserialized needs to be registered.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <rttr/registration>
#include <Prefabs/VariantPrefab.h>
#include <glm/glm.hpp>

static void rttr_auto_register_reflection_function_(); namespace {
  struct rttr__auto__register__ {
    rttr__auto__register__() {
      rttr_auto_register_reflection_function_();
    }
  };
} static const rttr__auto__register__ RTTR_CAT(auto_register__, __LINE__); static void rttr_auto_register_reflection_function_()
{
  /* ------------------- GLM ------------------- */
  rttr::registration::class_<glm::vec2>("Vec2")
    .constructor<>()
    .property("x", &glm::vec2::x)
    .property("y", &glm::vec2::y);
  rttr::registration::class_<glm::dvec2>("dVec2")
    .constructor<>()
    .property("x", &glm::dvec2::x)
    .property("y", &glm::dvec2::y);

  rttr::registration::class_<glm::vec3>("Vec3")
    .constructor<>()
    .property("x", &glm::vec3::x)
    .property("y", &glm::vec3::y)
    .property("z", &glm::vec3::z);
  rttr::registration::class_<glm::dvec3>("dVec3")
    .constructor<>()
    .property("x", &glm::dvec3::x)
    .property("y", &glm::dvec3::y)
    .property("z", &glm::dvec3::z);

  rttr::registration::class_<glm::vec4>("Vec4")
    .constructor<>()
    .property("x", &glm::vec4::x)
    .property("y", &glm::vec4::y)
    .property("z", &glm::vec4::z)
    .property("w", &glm::vec4::w);
  rttr::registration::class_<glm::dvec4>("dVec4")
    .constructor<>()
    .property("x", &glm::dvec4::x)
    .property("y", &glm::dvec4::y)
    .property("z", &glm::dvec4::z)
    .property("w", &glm::dvec4::w);

 /* rttr::registration::class_<JPH::BodyID>("JPH_BodyID")
    .constructor<JPH::uint32>()(
      rttr::parameter_names("mID")
    )
    .property_readonly("mID", JPH::BodyID::GetIndexAndSequenceNumber)
    ;*/

//#ifndef IMGUI_DISABLE
  if (Application::GetImGuiEnabled()) {
    rttr::registration::class_<std::pair<std::string, unsigned>>("StringUnsignedPair")
      .property("first", &std::pair<std::string, unsigned>::first)
      .property("second", &std::pair<std::string, unsigned>::second)
      ;

    rttr::registration::class_<Prefabs::VariantPrefab::EntityMappings>("EntityMappings")
      .property("name", &Prefabs::VariantPrefab::EntityMappings::mPrefab)
      .property("version", &Prefabs::VariantPrefab::EntityMappings::mVersion)
      .property("objToEntity", &Prefabs::VariantPrefab::EntityMappings::mObjToEntity)
      .property("registered", &Prefabs::VariantPrefab::EntityMappings::mRegistered)
      ;

    rttr::registration::class_<Prefabs::VariantPrefab::RemovedComponent>("RemovedComponent")
      .property("id", &Prefabs::VariantPrefab::RemovedComponent::mId)
      .property("type", &Prefabs::VariantPrefab::RemovedComponent::mType)
      .property("version", &Prefabs::VariantPrefab::RemovedComponent::mVersion)
      ;

    rttr::registration::class_<std::pair<Prefabs::PrefabSubData::SubDataId, Prefabs::PrefabVersion>>("UnsignedUnsignedPair")
      .property("first", &std::pair<Prefabs::PrefabSubData::SubDataId, Prefabs::PrefabVersion>::first)
      .property("second", &std::pair<Prefabs::PrefabSubData::SubDataId, Prefabs::PrefabVersion>::second)
      ;

    rttr::registration::class_<rttr::type>("RttrType");
  }
//#endif
}
