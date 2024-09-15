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
#include "JsonKeys.h"
#include <fstream>
#include <rapidjson/istreamwrapper.h>
#include <sstream>
#include <cstdarg>
#include <Prefabs/PrefabManager.h>

#define DESERIALIZER_DEBUG

#ifdef _DEBUG
std::ostream& operator<<(std::ostream& os, rttr::type const& type)
{
  os << type.get_name().to_string();

  return os;
}
#endif

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

  Reflection::ObjectFactory::EntityDataContainer Deserializer::DeserializeScene(std::string const& filepath)
  {
    rapidjson::Document document;
    // some initial sanity checks
    // we don't throw here because this is called when the engine
    // first runs, before the catching is established
    if (!ParseJsonIntoDocument(document, filepath)) {
      return{};
    }

    if (!document.IsArray()) {
      // log(filepath + ": root is not an array!");
#ifdef _DEBUG
      std::cout << filepath + ": root is not an array!" << "\n";
#endif
      return {};
    }
    // check if scn file contains all basic keys
    if (!ScanJsonFileForMembers(document, filepath, 6,
      JsonNameKey, rapidjson::kStringType, JsonChildEntitiesKey, rapidjson::kArrayType,
      JsonIdKey, rapidjson::kNumberType, JsonParentKey, rapidjson::kNumberType,
      JsonComponentsKey, rapidjson::kArrayType, JsonEntityStateKey, rapidjson::kFalseType))
    {
      return {};
    }

    // okay code starts here
    Reflection::ObjectFactory::EntityDataContainer ret{};
    for (auto const& entity : document.GetArray())
    {
      EntityID entityId{ entity[JsonIdKey].GetUint() };
      EntityID const parentId{ entity[JsonParentKey].IsNull() ? entt::null : entity[JsonParentKey].GetUint() };
      Reflection::VariantEntity entityVar{ entity[JsonNameKey].GetString(),
        parentId, entity[JsonEntityStateKey].GetBool() };  // set parent
      // get child ids
      for (auto const& child : entity[JsonChildEntitiesKey].GetArray()) {
        entityVar.mChildEntities.emplace_back(EntityID(child.GetUint()));
      }

#ifndef IMGUI_DISABLE
      // @TODO: TO RESTORE PREFAB SYSTEM
      /*Prefabs::PrefabManager& pm{ Prefabs::PrefabManager::GetInstance() };
      if (entity.HasMember(JsonPrefabKey) && !entity[JsonPrefabKey].IsNull())
      {
        rttr::variant mappedData{ Prefabs::VariantPrefab::EntityMappings{} };
        DeserializeRecursive(mappedData, entity[JsonPrefabKey]);
        pm.AttachPrefab(entityId, std::move(mappedData.get_value<Prefabs::VariantPrefab::EntityMappings>()));
      }*/
#endif

      // restore components
      std::vector<rttr::variant>& compVector{ entityVar.mComponents };
      for (auto const& elem : entity[JsonComponentsKey].GetArray())
      {
        for (rapidjson::Value::ConstMemberIterator comp{ elem.MemberBegin() }; comp != elem.MemberEnd(); ++comp)
        {
          std::string const compName{ comp->name.GetString() };
          rapidjson::Value const& compJson{ comp->value };

          rttr::type compType = rttr::type::get_by_name(compName);
#ifdef DESERIALIZER_DEBUG
          std::cout << "  [S] Deserializing " << compType << "\n";
#endif
          if (!compType.is_valid())
          {
            std::ostringstream oss{};
            oss << "Trying to deserialize an invalid component: " << compName;
            // log(oss.str());
#ifdef _DEBUG
            std::cout << oss.str() << "\n";
#endif
            continue;
          }

          rttr::variant compVar{};
          DeserializeComponent(compVar, compType, compJson);

          compVector.emplace_back(std::move(compVar));
        }
      }

      ret.emplace_back(entityId, std::move(entityVar));
    }

    return ret;
  }

  void Deserializer::DeserializeComponent(rttr::variant& compVar, rttr::type const& compType, rapidjson::Value const& compJson)
  {
#ifdef DESERIALIZER_DEBUG
    std::cout << "  Type of component variant is " << compType << "\n";
#endif

    rttr::constructor const& compCtr{ *compType.get_constructors().begin() };
    // if ctor is default, invoke it and proceed
    if (compCtr.get_parameter_infos().empty())
    {
      compVar = compCtr.invoke();
#ifdef DESERIALIZER_DEBUG
      std::cout << "  Invoking default ctor...\n";
#endif
      DeserializeRecursive(compVar, compJson);
    }
    else
    {
#ifdef DESERIALIZER_DEBUG
      std::cout << "  Invoking non-default ctor...\n";
#endif

      std::vector<rttr::argument> args{}; // vector of args for ctor
      // iterate through data members of component
      auto const properties{ compType.get_properties() };
      args.reserve(compCtr.get_parameter_infos().size());
      for (auto const& param : compCtr.get_parameter_infos())
      {
        for (auto& prop : properties)
        {
          // match param name with property name
          // to get the right order of args to invoke ctor
          if (param.get_name() == prop.get_name())
          {
#ifdef DESERIALIZER_DEBUG
            std::cout << "    Extracting property: " << prop.get_name().to_string() << " of type: " << prop.get_type() << "\n";
#endif
            rapidjson::Value::ConstMemberIterator iter{ compJson.FindMember(prop.get_name().to_string().c_str()) };
            if (iter == compJson.MemberEnd())
            {
              std::ostringstream oss{};
              oss << "Unable to find " << prop.get_name().to_string()
                << " property in " << compType.get_name().to_string();
              // log(oss.str());
              continue;
            }

            rttr::type const propType{ prop.get_type() };
            args.emplace_back(ExtractValue(iter->value, propType));
#ifdef DESERIALIZER_DEBUG
            std::cout << "    Added " << param.get_name() << " of type " << args.back().get_type() << " to args list\n";
#endif
            break;
          }
        }
      }

      compVar = compCtr.invoke_variadic(args);
#ifdef DESERIALIZER_DEBUG
      std::cout << "    Invoked ctor, returning " << compVar.get_type() << "\n";
#endif
    }
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

  bool Deserializer::ParseJsonIntoDocument(rapidjson::Document& document, std::string const& filepath)
  {
    std::ifstream ifs{ filepath };
    if (!ifs)
    {
      // log ("Unable to read " + filepath);
#ifdef _DEBUG
      std::cout << "Unable to read " << filepath << "\n";
#endif
      return false;
    }
    // parse into document object
    rapidjson::IStreamWrapper isw{ ifs };
    if (ifs.peek() == std::ifstream::traits_type::eof())
    {
      ifs.close(); //log ("Empty scene file read. Ignoring checks");
      return false;
    }

    if (document.ParseStream(isw).HasParseError())
    {
      ifs.close(); // log ("Unable to parse " + filepath);
#ifdef _DEBUG
      std::cout << "Unable to parse " + filepath << "\n";
#endif
      return false;
    }

    return true;
  }

  bool Deserializer::ScanJsonFileForMembers(rapidjson::Value const& value, std::string const& filename, unsigned keyCount, ...)
  {
    va_list args;
    va_start(args, keyCount);

    bool status{ true };
    std::vector <std::pair<std::string, rapidjson::Type>> keys{};
    for (unsigned i{}; i < keyCount; ++i)
    {
      std::string test{ va_arg(args, const char*) };
      keys.emplace_back(std::move(test), static_cast<rapidjson::Type>(va_arg(args, int)));
    }

    if (value.IsArray())
    {
      for (unsigned i{}; i < value.GetArray().Size(); ++i)
      {
        for (auto const& [keyName, type] : keys)
        {
          rapidjson::Value const& elem{ value[i] };
          auto result{ elem.FindMember(keyName.c_str()) };
          if (result == elem.MemberEnd())
          {
            std::ostringstream oss{};
            oss << filename << ": Unable to find key \"" + keyName + "\" of element: " << i << " in rapidjson value";
            // log
#ifdef _DEBUG
            std::cout << oss.str() << "\n";
#endif
            status = false;
            continue;
          }

          if ((type == rapidjson::kTrueType || type == rapidjson::kFalseType))
          {
            if (!elem[keyName.c_str()].IsBool())
            {
              std::ostringstream oss{};
              oss << filename << ": Element \"" << keyName << "\" is not of type bool";
              // log
#ifdef _DEBUG
              std::cout << oss.str() << "\n";
#endif
              status = false;
            }
          }
          else if (!elem[keyName.c_str()].IsNull() && elem[keyName.c_str()].GetType() != type)
          {
            std::ostringstream oss{};
            oss << filename << ": Element \"" << keyName << "\" is not of rapidjson type:" << type;
            // log
#ifdef _DEBUG
            std::cout << oss.str() << "\n";
#endif
            status = false;
          }
        }
      }
    }
    else
    {
      for (auto const& [keyName, type] : keys)
      {
        auto result{ value.FindMember(keyName.c_str()) };
        if (result == value.MemberEnd())
        {
          std::ostringstream oss{};
          oss << filename << ": Unable to find key \"" << keyName << "\" in rapidjson value";
          // log
#ifdef _DEBUG
          std::cout << oss.str() << "\n";
#endif
          status = false;
          continue;
        }

        if ((type == rapidjson::kTrueType || type == rapidjson::kFalseType))
        {
          if (!value[keyName.c_str()].IsBool())
          {
            std::ostringstream oss{};
            oss << filename << ": Element \"" << keyName << "\" is not of type bool";
            // log
#ifdef _DEBUG
            std::cout << oss.str() << "\n";
#endif
            status = false;
          }
        }
        else if (!value[keyName.c_str()].IsNull() && value[keyName.c_str()].GetType() != type)
        {
          std::ostringstream oss{};
          oss << filename << ": Element \"" << keyName << "\" is not of rapidjson type:" << type;
          // log
#ifdef _DEBUG
          std::cout << oss.str() << "\n";
#endif
          status = false;
        }
      }
    }

    va_end(args);
    return status;
  }

} // namespace Serialization
