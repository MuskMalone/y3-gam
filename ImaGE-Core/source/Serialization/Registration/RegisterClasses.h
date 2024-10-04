/*!*********************************************************************
\file   RegisterClasses.cpp
\date   15-September-2024
\brief  This file handles the registration of custom classes/structs
        used in our engine to be recognized by RTTR library. Anything
        that needs to be serialized/deserialized needs to be registered.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <rttr/registration>
#include <Prefabs/Prefab.h>

static void rttr_auto_register_reflection_function_(); namespace {
  struct rttr__auto__register__ {
    rttr__auto__register__() {
      rttr_auto_register_reflection_function_();
    }
  };
} static const rttr__auto__register__ auto_register__19; static void rttr_auto_register_reflection_function_()
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

  rttr::registration::class_<glm::quat>("GLMQuat")
    .constructor<>()
    .property("x", &glm::quat::x)
    .property("y", &glm::quat::y)
    .property("z", &glm::quat::z)
    .property("w", &glm::quat::w);
  
  /* ------------------- Other ------------------- */
  rttr::registration::class_<rttr::type>("RttrType");

  //rttr::registration::class_<JPH::Vec3>("JPHVec3")
  //  .constructor<>()
  //  .property("x", &JPH::Vec3::GetX, &JPH::Vec3::SetX)
  //  .property("y", &JPH::Vec3::GetY, &JPH::Vec3::SetY)
  //  .property("z", &JPH::Vec3::GetZ, &JPH::Vec3::SetZ);

  if (IGE::Application::GetImGuiEnabled()) {
    rttr::registration::class_<std::pair<std::string, unsigned>>("StringUnsignedPair")
      .property("first", &std::pair<std::string, unsigned>::first)
      .property("second", &std::pair<std::string, unsigned>::second)
      ;
  }
}
