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
#include <pch.h>
#include "Deserializer.h"
#include <fstream>
#include <rapidjson/istreamwrapper.h>

namespace Serialization
{

  void Deserializer::DeserializeAny(rttr::instance inst, std::string const& filename)
  {
    std::ifstream ifs{ filename };
    if (!ifs)
    {
#ifdef _DEBUG
      std::cout << "Unable to read " << filename << "\n";
#endif
      return;
    }

    rapidjson::Document document{};
    rapidjson::IStreamWrapper isw{ ifs };
    if (document.ParseStream(isw).HasParseError())
    {
      ifs.close();
#ifdef _DEBUG
      std::cout << "Unable to parse " + filename << "\n";
#endif
      return;
    }

    DeserializeRecursive(inst, document);
  }

  void Deserializer::DeserializeRecursive(rttr::instance inst, rapidjson::Value const& jsonObj)
  {
    rttr::instance wrappedInst{ inst.get_type().get_raw_type().is_wrapper() ? inst.get_wrapped_instance() : inst };
    for (auto const& prop : inst.get_derived_type().get_properties())
    {
      auto ret{ jsonObj.FindMember(prop.get_name().data()) };
      if (ret == jsonObj.MemberEnd())
      {
#ifdef _DEBUG
        std::cout << "Unable to find property with name: " << prop.get_name().to_string() << "\n";
#endif
        continue;
      }

      rttr::type const propType{ prop.get_type() };
      auto const& jsonVal{ ret->value };
      switch (jsonVal.GetType())
      {
        // if its a container
      case rapidjson::kArrayType:
      {
        rttr::variant var{};
        if (propType.is_sequential_container())
        {
          var = prop.get_value(wrappedInst);
          auto seqView{ var.create_sequential_view() };
          DeserializeSequentialContainer(seqView, jsonVal);
        }
        else if (propType.is_associative_container())
        {
          var = prop.get_value(wrappedInst);
          auto associativeView{ var.create_associative_view() };
          DeserializeAssociativeContainer(associativeView, jsonVal);
        }

        prop.set_value(wrappedInst, var);
        break;
      }
      // if its a class object
      case rapidjson::kObjectType:
      {
        rttr::variant var{ prop.get_value(wrappedInst) };
        DeserializeRecursive(var, jsonVal);

        prop.set_value(wrappedInst, var);
        break;
      }
      // else its a basic type
      default:
      {
        rttr::variant extractedVal{ ExtractBasicTypes(jsonVal) };
        if (!extractedVal.convert(propType))
        {
#ifdef _DEBUG
          std::cout << "Unable to convert element to type " << propType.get_name().to_string() << "\n";
#endif
          continue;
        }

        prop.set_value(wrappedInst, extractedVal);
        break;
      }
      }
    }
  }

  rttr::variant Deserializer::ExtractBasicTypes(rapidjson::Value const& jsonVal)
  {
    switch (jsonVal.GetType())
    {
    case rapidjson::kStringType:
      return std::string(jsonVal.GetString());
    case rapidjson::kTrueType:
    case rapidjson::kFalseType:
      return jsonVal.GetBool();
    case rapidjson::kNumberType:
    {
      if (jsonVal.IsUint())
        return jsonVal.GetUint();
      else if (jsonVal.IsInt())
        return jsonVal.GetInt();
      else if (jsonVal.IsDouble())
        return jsonVal.GetDouble();
      else if (jsonVal.IsInt64())
        return jsonVal.GetInt64();
      else if (jsonVal.IsUint64())
        return jsonVal.GetUint64();
      break;
    }

    case rapidjson::kNullType:
    case rapidjson::kObjectType:
    case rapidjson::kArrayType:
    default:
      break;
    }

    return rttr::variant();
  }

  void Deserializer::DeserializeSequentialContainer(rttr::variant_sequential_view& seqView, rapidjson::Value const& jsonVal)
  {
    seqView.set_size(jsonVal.Size());
    for (rapidjson::SizeType i{}; i < jsonVal.Size(); ++i)
    {
      rapidjson::Value const& idxVal{ jsonVal[i] };
      // if still an array, call this function again
      if (idxVal.IsArray())
      {
        auto subArrView{ seqView.get_value(i).create_sequential_view() };
        DeserializeSequentialContainer(subArrView, idxVal);
      }
      // if its a class, deserialize each of its members
      else if (idxVal.IsObject())
      {
        rttr::variant wrappedVal{ seqView.get_value(i).extract_wrapped_value() };
        DeserializeRecursive(wrappedVal, idxVal);
        //seqView.set_value(i, result);
      }
      // else its a basic type
      else
      {
        rttr::variant result{ ExtractBasicTypes(idxVal) };
        if (!seqView.set_value(i, result))
        {
#ifdef _DEBUG
          std::cout << "Unable to set sequential view of type " << seqView.get_type().get_name().to_string() << "\n";
#endif
        }
      }
    }
  }

  rttr::variant Deserializer::ExtractValue(rapidjson::Value const& jsonVal, rttr::type const& type)
  {
    rttr::variant extractedVal{ ExtractBasicTypes(jsonVal) };
    if (!extractedVal.convert(type))
    {
      if (jsonVal.IsObject())
      {
        rttr::constructor ctor{ type.get_constructor() };
        for (auto& elem : type.get_constructors())
        {
          if (elem.get_instantiated_type() == type)
          {
            ctor = elem;
            break;
          }
        }
        extractedVal = ctor.invoke();
        DeserializeRecursive(extractedVal, jsonVal);
      }
    }

    return extractedVal;
  }

  void Deserializer::DeserializeAssociativeContainer(rttr::variant_associative_view& view, rapidjson::Value const& jsonVal)
  {
    for (rapidjson::SizeType i{}; i < jsonVal.Size(); ++i)
    {
      auto& idxVal{ jsonVal[i] };
      // if key-value pair
      if (idxVal.IsObject())
      {
        auto keyIter{ idxVal.FindMember("key") }, valIter{ idxVal.FindMember("value") };

        if (keyIter == idxVal.MemberEnd() || valIter == idxVal.MemberEnd())
        {
#ifdef _DEBUG
          std::cout << "[Associative View] Unable to find key-value pair for element of type " << view.get_key_type().get_name().to_string()
            << "-" << view.get_value_type().get_name().to_string() << "\n";
#endif
        }

        auto keyVar{ ExtractValue(keyIter->value, view.get_key_type()) }, valVar{ ExtractValue(valIter->value, view.get_value_type()) };
        if (!keyVar || !valVar)
        {
#ifdef _DEBUG
          std::cout << "[Associative View] Unable to extract key-value pair for element of type " << view.get_key_type().get_name().to_string()
            << "-" << view.get_value_type().get_name().to_string() << "\n";
#endif
        }
        else
        {
          auto result{ view.insert(keyVar, valVar) };
          if (!result.second)
          {
#ifdef _DEBUG
            std::cout << "[Associative View] Unable to insert key-value pair for element of type " << view.get_key_type().get_name().to_string()
              << "-" << view.get_value_type().get_name().to_string() << "\n";
            std::cout << "Types are " << keyVar.get_type().get_name().to_string() << " and " << valVar.get_type().get_name().to_string() << "\n";
#endif
          }
        }
      }
      // else if key-only
      else
      {
        rttr::variant extractedVal{ ExtractBasicTypes(idxVal) };
        if (!extractedVal || !extractedVal.convert(view.get_key_type()))
        {
#ifdef _DEBUG
          std::cout << "[Associative View] Unable to extract key-only type of " << view.get_key_type().get_name().to_string() << "\n";
#endif
        }

        auto result{ view.insert(extractedVal) };
        if (!result.second)
        {
#ifdef _DEBUG
          std::cout << "[Associative View] Unable to insert key-only type of " << view.get_key_type().get_name().to_string() << "\n";
#endif
        }
      }
    }
  }

} // namespace Serialization
