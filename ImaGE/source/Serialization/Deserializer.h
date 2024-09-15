/*!*********************************************************************
\file   Deserializer.h
\author chengen.lau\@digipen.edu
\date   10-September-2024
\brief  The class encapsulating functions related to deserialization.
        Implementation makes use of RTTR reflection library. Applies a
        standard algorithm of recursion to work for for sequential
        containers with any type and size. This is so that not much
        code has to be modified when any components/classes are changed.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <rapidjson/document.h>
#include <rttr/variant.h>
#include <string>

namespace Serialization
{
  class Deserializer
  {
  public:
    Deserializer() = delete;

    static void DeserializeAny(rttr::instance inst, std::string const& filename);

  private:
    static void DeserializeRecursive(rttr::instance inst, rapidjson::Value const& jsonObj);
    static rttr::variant ExtractBasicTypes(rapidjson::Value const& jsonVal);
    static void DeserializeSequentialContainer(rttr::variant_sequential_view& seqView, rapidjson::Value const& jsonVal);
    static rttr::variant ExtractValue(rapidjson::Value const& jsonVal, rttr::type const& type);
    static void DeserializeAssociativeContainer(rttr::variant_associative_view& view, rapidjson::Value const& jsonVal);
  };

} // namespace Serialization