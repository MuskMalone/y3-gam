/*!*********************************************************************
\file   RegisterClasses.cpp
\date   10-September-2024
\brief  This file handles the registration of custom classes/structs
        used in our engine to be recognized by RTTR library. Anything
        that needs to be serialized/deserialized needs to be registered.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include <rttr/registration>

RTTR_REGISTRATION
{
  using namespace rttr;
  /*registration::class_<NewComponent>("NewComponent")
    .property("vecOfStrings", &NewComponent::vecOfStrings)
    .property("setOfFloats", &NewComponent::setOfFloats)
    .property("mapOfIntStr", &NewComponent::mapOfIntStr);*/
}
