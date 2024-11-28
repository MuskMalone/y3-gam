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
#include <string>
#include <Prefabs/Prefab.h>

namespace Serialization
{
  enum FileFormat : uint8_t {
    COMPACT,
    PRETTY
  };

  class Serializer
  {
  public:
    Serializer() = delete;

    /*!*********************************************************************
    \brief
      Serializes an rttr::variant object of any type into a json file.
      Current implementation can support almost any form of sequential
      containers such as std::vector.
    \param inst
      The rttr::instance containing the object to serialize
    \param filePath
      The path of the ouput file
    ************************************************************************/
    static void SerializeAny(rttr::instance const& inst, std::string const& filePath, FileFormat format = FileFormat::PRETTY);

    /*!*********************************************************************
    \brief
      Serializes a scene into a json file. Calls other private function
      to serialize each entity, their components and their members.
    \param filePath
      The name of the output file
    ************************************************************************/
    static void SerializeScene(std::string const& filePath, FileFormat format = FileFormat::PRETTY);

#ifndef IMGUI_DISABLE
    /*!*********************************************************************
    \brief
      Serializes a VariantPrefab object into a json file. The VariantPrefab
      encapsulates a proxy entity by storing components in a vector each
      with the form of an rttr::variant.
    \param prefab
      The VariantPrefab object
    \param filename
      The name of the output file
    ************************************************************************/
    static void SerializePrefab(Prefabs::Prefab const& prefab, std::string const& filename, FileFormat format = FileFormat::PRETTY);
#endif

  private:
  };
} // namespace Serialization
