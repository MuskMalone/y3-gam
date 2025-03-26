/*!*********************************************************************
\file   ConverterFuncs.h
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  This file handles the registration of converter functions
        used in our engine to be recognized by RTTR library.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <rttr/policy.h>
#include <rttr/type.h>

namespace RTTRHelper {
  std::string StringFromRttrType(rttr::type const& type, bool& ok) {
    ok = true; return type.get_name().to_string();
  }

  rttr::type RttrTypeFromString(std::string const& str, bool& ok) {
    ok = true; return rttr::type::get_by_name(str);
  }
}
