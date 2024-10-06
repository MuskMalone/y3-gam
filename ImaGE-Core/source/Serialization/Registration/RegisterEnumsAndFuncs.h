/*!*********************************************************************
\file   RegisterEnumsAndFuncs.cpp
\date   15-September-2024
\brief  This file handles the registration of converter functions and
        enumerations used in our engine to be recognized by RTTR library.
        Anything that needs to be serialized/deserialized needs to be
        registered.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <rttr/type.h>
#include <rttr/registration>
#include <Core/Components/RigidBody.h>

namespace
{
  std::string StringFromRttrType(rttr::type const& type, bool& ok) {
    ok = true; return type.get_name().to_string();
  }

  rttr::type RttrTypeFromString(std::string const& str, bool& ok) {
    ok = true; return rttr::type::get_by_name(str);
  }
}

static void rttr_auto_register_reflection_function3_(); namespace {
  struct rttr__auto__register3__ {
    rttr__auto__register3__() {
      rttr_auto_register_reflection_function3_();
    }
  };
} static const rttr__auto__register3__ RTTR_CAT(auto_register__, __LINE__); static void rttr_auto_register_reflection_function3_()
{
  /* ------------------- FUNCTIONS ------------------- */
  rttr::type::register_converter_func(StringFromRttrType);
  rttr::type::register_converter_func(RttrTypeFromString);

  /* ------------------- ENUMERATIONS ------------------- */
  rttr::registration::enumeration<Component::RigidBody::MotionType>("MotionType")(
    rttr::value("DYNAMIC", Component::RigidBody::MotionType::DYNAMIC),
    rttr::value("KINEMATIC", Component::RigidBody::MotionType::KINEMATIC)
    );
}
