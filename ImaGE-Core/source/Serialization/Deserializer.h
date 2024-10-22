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
#include <Reflection/ObjectFactory.h>
#include <Prefabs/Prefab.h>

namespace Serialization
{
  class Deserializer
  {
  public:
    Deserializer() = delete;

    /*!*********************************************************************
    \brief
      General purpose function. Deserializes a json file into an object of
      any type. Current implementation can support almost any form of class
      as long as it is registered to the system.
    \param inst
      The instance of the object to deserialize into
    \param filePath
      The path of the file
    ************************************************************************/
    static void DeserializeAny(rttr::instance inst, std::string const& filePath);

    /*!*********************************************************************
    \brief
      Main function called to deserialize a scene file. Returns a vector
      of std::pair of entity ID and VariantEntity objects encapsulating
      the deserialized data read from the file. When a missing field or
      invalid type is encountered, the function logs an error to the
      console and skips the element.
    \param filepath
      The scene file to deserialize
    \return
      An std::vector<std::pair<EntityID,VariantEntity>> with the
      deserailized data
    ************************************************************************/
    static void DeserializeScene(std::vector<Reflection::VariantEntity>& entities,
      Reflection::ObjectFactory::PrefabInstanceContainer& prefabInstances, std::string const& filepath);

    /*!*********************************************************************
     \brief
       Deserializes a prefab from a json file into a Prefab object.
       It contains the prefab name along with a vector of its components
       stored in an rttr::variant object.
     \param json
       The file containing the prefab data
     \return
       The Prefab object
     ************************************************************************/
    static Prefabs::Prefab DeserializePrefabToVariant(std::string const& json);

  private:
    using EntityID = ECS::EntityManager::EntityID;

    /*!*********************************************************************
    \brief
      This function handles the deserialization of a component. Based on
      the string value of a component read from the file, an instance
      of the class will be constructed through RTTR and then passed on
      to other functions to deserialize its data members.
    \param compVar
      The rttr::variant to deserialize into
    \param compType
      The type of the component
    \param compJson
      The json data corresponding to the component
    ************************************************************************/
    static void DeserializeComponent(rttr::variant& compVar, rttr::type const& compType, rapidjson::Value const& compJson);

    /*!*********************************************************************
    \brief
      Handles classes that require custom deserialization.
    \param var
      The object to deserialize
    \param type
      The type of the object
    \param jsonVal
      The rapidjson value
    \return
      True if the object was deserialized and false otherwise
    ************************************************************************/
    static bool DeserializeSpecialCases(rttr::variant& var, rttr::type const& type, rapidjson::Value const& jsonVal);

    /*!*********************************************************************
    \brief
      Deserializes the PrefabOverrides component
    \param prefabOverride
      Reference to the component
    \param json
      The json value of the component
    ************************************************************************/
    static void DeserializePrefabOverrides(Component::PrefabOverrides& prefabOverride, rapidjson::Value const& json);

    /*!*********************************************************************
    \brief
      Main function being called when deserializing to classify the given
      rttr::variant's type along with its corresponding rapidjson::value.
      Based on the result, the respective helper function is called to
      proceed with the deserialization process
    \param inst
      The instance of the object to read the data into
    \param jsonObj
      The json data corresponding to the object
    ************************************************************************/
    static void DeserializeRecursive(rttr::instance inst, rapidjson::Value const& jsonObj);

    /*!*********************************************************************
    \brief
      This function handles deserializing of basic types such as ints,
      floats, strings,, bools and doubles. The type is determined by
      the rapidjson::Value and a rttr::variant containing the object
      is returned
    \param jsonVal
      The json data of the object
    \return
      The deserialized object encapsulated withint an rttr::variant
    ************************************************************************/
    static rttr::variant ExtractBasicTypes(rapidjson::Value const& jsonVal);

    /*!*********************************************************************
    \brief
      This function handles the deserialization of sequential containers
      (such as std::vector). It makes use of recursion to handle nested
      containers. This allows it to deserialized the object until it
      reaches the underlying element.
    \param view
      The sequential_view of the container
    \param value
      The json data of the container
    ************************************************************************/
    static void DeserializeSequentialContainer(rttr::variant_sequential_view& seqView, rapidjson::Value const& jsonVal);

    /*!*********************************************************************
    \brief
      Used when deserializing an associative container to extract its
      element.
    \param jsonVal
      The json data of the container
    \param type
      The rttr::type of the object
    \return
      The corresponding value as an rttr::variant
    ************************************************************************/
    static rttr::variant ExtractValue(rapidjson::Value const& jsonVal, rttr::type const& type);

    /*!*********************************************************************
    \brief
      Handles the deserialization of associative containers (such as
      std::map). It uses the strings defined in JsonKeys.h to retrieve the
      keys and values of each entry.
    \param view
      The associative_view of the container
    \param jsonVal
      The json data of the container
    ************************************************************************/
    static void DeserializeAssociativeContainer(rttr::variant_associative_view& view, rapidjson::Value const& jsonVal);

    /*!*********************************************************************
    \brief
      This function handles the deserialization of the scripts component
    \param var
      The rttr::variant to deserialize into
    \param jsonVal
      The json data
    ************************************************************************/
    static void DeserializeProxyScript(rttr::variant& var, rapidjson::Value const& jsonVal);

    /*!*********************************************************************
      \brief
        Parses a json file into a rapidjson::Value or Document object.
        Uses IStreamWrapper to do so and reports any errors encountered.
      \param value
        The rapidjson object to read into
      \param filepath
       The json file to read from
      \return
        True if the operation was successful and false otherwise
      ************************************************************************/
    static bool ParseJsonIntoDocument(rapidjson::Document& document, std::string const& filepath);

    /*!*********************************************************************
      \brief
        Variadic function to validate the format of a json file.
        It iterates through the document and validates that each
        key specified is of type equal to the rapidjson::Type passed in after
        that key in the argument list.
        keyCount is the number of key-value pairs to validate, meaning
        the number of args in the (...) should be = keyCount * 2
        Each key should be followed by its rapidjson::Type in the argument
        list. Note that if the value is null, it will be ignored.

        Example usage:
          For a file that contains the following key-value pairs:
            "Name": "string" and "Components": [ "bla", "bla" ]
          You would invoke the function with:
            ScanJsonFileForMembers(jsonValue, 2,
              "Name", rapidjson::kStringType,
              "Components", rapidjson::kArrayType)
      \param value
        The rapidjson::Value to validate
      \param filename
        The name of the file (used for printing err msg)
      \param keyCount
        The number of fields (key-value pairs) to check
      \param ...
        The comma-separated argument list in pairs of key followed by
        its corresponding type i.e. [const char*, rapidjson::Type]
      \return
        True if there were no missing fields and false otherwise
      ************************************************************************/
    static bool ScanJsonFileForMembers(rapidjson::Value const& value, std::string const& filename, unsigned keyCount, ...);

    /*!*********************************************************************
    \brief
      This function attempts to deserialize a rapidjson::Value into an
      int object. This is used to help distinguish between signed and
      unsigned ints.
    \param var
      The object to deserialize into
    \param value
      The json data of the object
    ************************************************************************/
    static bool TryDeserializeIntoInt(rttr::variant& var, rapidjson::Value const& value);
  };

} // namespace Serialization