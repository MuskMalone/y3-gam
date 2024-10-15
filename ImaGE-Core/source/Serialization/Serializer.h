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
#include <Prefabs/Prefab.h>

// forward declaration
namespace ECS { class Entity; }

namespace Serialization
{

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
    static void SerializeAny(rttr::instance const& inst, std::string const& filePath);

    /*!*********************************************************************
    \brief
      Serializes a scene into a json file. Calls other private function
      to serialize each entity, their components and their members.
    \param filePath
      The name of the output file
    ************************************************************************/
    static void SerializeScene(std::string const& filePath);

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
    static void SerializePrefab(Prefabs::Prefab const& prefab, std::string const& filename);
#endif

  private:
    using WriterType = rapidjson::PrettyWriter<rapidjson::OStreamWrapper>;

    /*!*********************************************************************
    \brief
      Helper function to serialize an entity's properties
    \param entity
      The entity to serialize
    \param writer
      The writer to write to
    ************************************************************************/
    static void SerializeEntity(ECS::Entity const& entity, WriterType& writer);

    /*!*********************************************************************
    \brief
      Helper function to serialize a vector of components of an entity
    \param components
      The vector of components
    \param writer
      The writer to write to
    ************************************************************************/
    static void SerializeVariantComponents(std::vector<rttr::variant> const& components, WriterType& writer);

    /*!*********************************************************************
    \brief
      Serializes a class by iterating through its properties
    \param object
      The object to serialize
    \param writer
      The writer to write to
    ************************************************************************/
    static void SerializeClassTypes(rttr::instance const& obj, WriterType& writer);

    /*!*********************************************************************
    \brief
      Handles classes that require custom serialization. This should be
      called before the standard SerializeClassTypes.
    \param object
      The object to serialize
    \param type
      The type of the object
    \param writer
      The writer to write to
    \return
      True if the object was serialized and false otherwise
    ************************************************************************/
    static bool SerializeSpecialCases(rttr::instance const& obj, rttr::type const& type, WriterType& writer);

    /*!*********************************************************************
    \brief
      Serializes a basic type into the writer
    \param valueType
      The rttr::type of the object
    \param value
      The rttr::variant of the object
    \param writer
      The writer to write to
    \return
      True if the value was serialized and false otherwise
    ************************************************************************/
    static bool WriteBasicTypes(rttr::type const& type, rttr::variant const& var, WriterType& writer);

    /*!*********************************************************************
    \brief
      Serializes an rttr::variant containing a sequential container type
      (such as std::vector). Makes use of recursion to serialize a
      container till it reaches the base element
    \param seqView
      The sequential view of the container
    \param writer
      The writer to write to
    ************************************************************************/
    static void WriteSequentialContainer(rttr::variant_sequential_view const& seqView, WriterType& writer);

    /*!*********************************************************************
    \brief
      Serializes an rttr::variant into a rapidjson::Value object based
      on its type. (Whether its a C basic type, Enum or class type)
    \param var
      The rttr::variant of the object
    \param writer
      The writer to write to
    \return
      True if the object was serialized and false otherwise
    ************************************************************************/
    static bool SerializeRecursive(rttr::variant const& var, WriterType& writer);

    /*!*********************************************************************
    \brief
      Serializes an rttr::variant containing an associative container type
      (such as std::map)
    \param view
      The associative view of the container
    \param writer
      The writer to write to
    ************************************************************************/
    static void WriteAssociativeContainer(rttr::variant_associative_view const& view, WriterType& writer);
  };

} // namespace Serialization
