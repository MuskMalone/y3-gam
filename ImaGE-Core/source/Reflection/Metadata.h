/*!*********************************************************************
\file   Metadata.h
\author chengen.lau\@digipen.edu
\date   21-October-2024
\brief  Keys used to embed additional metadata into RTTR-registered
        properties
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once

namespace Reflection {

  enum InspectorMetadata {
    // no values needed
    DISABLED,
    HIDE,

    // glm::vec2
    FLOAT_MIN_MAX,  
    VEC_MIN_MAX,

    // not in use yet
    CUSTOM_DISPLAY
  };

} // namespace Reflection
