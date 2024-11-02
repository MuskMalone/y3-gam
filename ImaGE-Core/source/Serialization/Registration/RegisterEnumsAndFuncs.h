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
#include <Core/Components/Light.h>

#include <Audio/AudioManager.h>
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
  rttr::registration::enumeration<IGE::Audio::SoundInvokeSetting::RolloffType>("RolloffType")
      (
          rttr::value("LINEAR", IGE::Audio::SoundInvokeSetting::RolloffType::LINEAR),
          rttr::value("LOGARITHMIC", IGE::Audio::SoundInvokeSetting::RolloffType::LOGARITHMIC)
          );

  rttr::registration::enumeration<Component::LightType>("LightType")(
    rttr::value("DIRECTIONAL", Component::LightType::DIRECTIONAL),
    rttr::value("SPOTLIGHT", Component::LightType::SPOTLIGHT)
    );
}
