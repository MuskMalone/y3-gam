/*!*********************************************************************
\file   Serializer.h
\author chengen.lau\@digipen.edu
\date   10-September-2024
\brief  Contians the class encapsulating functions related to
        serialization. Implementation makes use of RTTR reflection
        library. Applies a standard algorithm of recursion
        to work for any type and size for sequential containers. This
        is so that not much code has to be modified when any
        components/classes are changed.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <rapidjson/document.h>
#include <rttr/variant.h>
#include <rapidjson/PrettyWriter.h>
#include <rapidjson/ostreamwrapper.h>
#include <string>

namespace Serialization
{

  class Serializer
  {
  public:
    Serializer() = delete;

    static void SerializeAny(rttr::instance const& obj, std::string const& filename);

  private:
    using WriterType = rapidjson::PrettyWriter<rapidjson::OStreamWrapper>;

    static void SerializeRecursive(rttr::instance const& obj, WriterType& writer);

    static bool WriteBasicTypes(rttr::type const& type, rttr::variant const& var, WriterType& writer);
    static void WriteSequentialContainer(rttr::variant_sequential_view const& seqView, WriterType& writer);
    static bool WriteVariant(rttr::variant const& var, WriterType& writer);
    static void WriteAssociativeContainer(rttr::variant_associative_view const& view, WriterType& writer);
  };

} // namespace Serialization
