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

  //using namespace JPH;
  ///* ------------------- ENUMERATIONS ------------------- */
  //rttr::registration::enumeration<JPH::EShapeSubType>("JPH_EShapeSubType")(
  //  rttr::value("Sphere", EShapeSubType::Sphere),
  //  rttr::value("Box", EShapeSubType::Box),
  //  rttr::value("Triangle", EShapeSubType::Triangle),
  //  rttr::value("Capsule", EShapeSubType::Capsule),
  //  rttr::value("TaperedCapsule", EShapeSubType::TaperedCapsule),
  //  rttr::value("Cylinder", EShapeSubType::Cylinder),
  //  rttr::value("ConvexHull", EShapeSubType::ConvexHull),
  //  rttr::value("StaticCompound", EShapeSubType::StaticCompound),
  //  rttr::value("MutableCompound", EShapeSubType::MutableCompound),
  //  rttr::value("Mesh", EShapeSubType::Mesh),
  //  rttr::value("HeightField", EShapeSubType::HeightField),
  //  rttr::value("SoftBody", EShapeSubType::SoftBody),
  //  rttr::value("SoftBody", EShapeSubType::Plane),
  //  rttr::value("TaperedCylinder", EShapeSubType::TaperedCylinder)
  //  );

  //rttr::registration::enumeration<JPH::EMotionType>("JPH_EMotionType")(
  //  rttr::value("Static", EMotionType::Static),
  //  rttr::value("Dynamic", EMotionType::Dynamic),
  //  rttr::value("Kinematic", EMotionType::Kinematic)
  //  );
}
