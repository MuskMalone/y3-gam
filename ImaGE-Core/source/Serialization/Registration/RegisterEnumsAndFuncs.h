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
#include <Core/Components/Animation.h>
#include <Core/Components/Video.h>
#include <Asset/SmartPointer.h>

#include <Audio/AudioManager.h>
namespace
{
  std::string StringFromRttrType(rttr::type const& type, bool& ok) {
    ok = true; return type.get_name().to_string();
  }

  rttr::type RttrTypeFromString(std::string const& str, bool& ok) {
    ok = true; return rttr::type::get_by_name(str);
  }

  IGE::Assets::GUID UintToGUID(unsigned val, bool& ok) {
    ok = true; return IGE::Assets::GUID(val);
  }

  IGE::Assets::GUID Uint64ToGUID(uint64_t val, bool& ok) {
    ok = true; return IGE::Assets::GUID(val);
  }

  int UintToInt(unsigned val, bool& ok) {
    ok = true; return static_cast<int>(val);
  }

  unsigned IntToUint(int val, bool& ok) {
    ok = true; return static_cast<unsigned>(val);
  }

  entt::entity UintToEnttEntity(unsigned val, bool& ok) {
    ok = true; return static_cast<entt::entity>(val);
  }

  unsigned EnttEntityToUint(entt::entity val, bool& ok) {
    ok = true; return static_cast<unsigned>(val);
  }

  float DoubleToFloat(double val, bool& ok) {
    ok = true; return static_cast<float>(val);
  }

  double FloatToDouble(float val, bool& ok) {
    ok = true; return static_cast<double>(val);
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
  rttr::type::register_converter_func(Uint64ToGUID);
  rttr::type::register_converter_func(UintToGUID);
  rttr::type::register_converter_func(UintToInt);
  rttr::type::register_converter_func(IntToUint);
  //rttr::type::register_converter_func(UintToEnttEntity);
  //rttr::type::register_converter_func(EnttEntityToUint);
  //rttr::type::register_converter_func(DoubleToFloat);
  //rttr::type::register_converter_func(FloatToDouble);

  /* ------------------- ENUMERATIONS ------------------- */
  //rttr::registration::enumeration<entt::entity>("enttEntity");

  rttr::registration::enumeration<Component::RigidBody::MotionType>("MotionType")(
    rttr::value("DYNAMIC", Component::RigidBody::MotionType::DYNAMIC),
    rttr::value("KINEMATIC", Component::RigidBody::MotionType::KINEMATIC)
  );
  rttr::registration::enumeration<Component::RigidBody::JointType>("JointType")(
    rttr::value("REVOLUTE", Component::RigidBody::JointType::REVOLUTE),
    rttr::value("SPHERICAL", Component::RigidBody::JointType::SPHERICAL),
    rttr::value("PRISMATIC", Component::RigidBody::JointType::PRISMATIC),
    rttr::value("DISTANCE", Component::RigidBody::JointType::DISTANCE)
  );
  rttr::registration::enumeration<IGE::Audio::SoundInvokeSetting::RolloffType>("RolloffType")(
    rttr::value("LINEAR", IGE::Audio::SoundInvokeSetting::RolloffType::LINEAR),
    rttr::value("LOGARITHMIC", IGE::Audio::SoundInvokeSetting::RolloffType::LOGARITHMIC),
    rttr::value("NONE", IGE::Audio::SoundInvokeSetting::RolloffType::NONE)
  );

  rttr::registration::enumeration<IGE::Audio::SoundInvokeSetting::PostProcessingType>("PostProcessingType")(
    rttr::value("REVERB", IGE::Audio::SoundInvokeSetting::PostProcessingType::REVERB),
    rttr::value("ECHO", IGE::Audio::SoundInvokeSetting::PostProcessingType::ECHO),
    rttr::value("DISTORTION", IGE::Audio::SoundInvokeSetting::PostProcessingType::DISTORTION),
    rttr::value("CHORUS", IGE::Audio::SoundInvokeSetting::PostProcessingType::CHORUS)
  );
  rttr::registration::enumeration<Component::LightType>("LightType")(
    rttr::value("DIRECTIONAL", Component::LightType::DIRECTIONAL),
    rttr::value("SPOTLIGHT", Component::LightType::SPOTLIGHT),
    rttr::value("POINT", Component::LightType::POINT)
  );

  {
    using T = Anim::KeyframeType;
    rttr::registration::enumeration<T>("KeyframeType")(
      rttr::value("NONE", T::NONE),
      rttr::value("TRANSLATION", T::TRANSLATION),
      rttr::value("ROTATION", T::ROTATION),
      rttr::value("SCALE", T::SCALE)
    );
  }
  {
    using T = Anim::InterpolationType;
    rttr::registration::enumeration<T>("InterpolationType")(
      rttr::value("LINEAR", T::LINEAR),
      rttr::value("EASE_IN", T::EASE_IN),
      rttr::value("EASE_OUT", T::EASE_OUT),
      rttr::value("EASE_INOUT", T::EASE_INOUT)
    );
  }

  rttr::registration::enumeration<Component::Camera::Type>("CameraType")(
    rttr::value("ORTHO", Component::Camera::Type::ORTHO),
    rttr::value("PERSP", Component::Camera::Type::PERSP)
  );

  {
    using T = Component::Video::RenderType;
    rttr::registration::enumeration<T>("VideoRenderType")(
      rttr::value("WORLD", T::WORLD),
      rttr::value("UI", T::UI)
    );
  }
}
