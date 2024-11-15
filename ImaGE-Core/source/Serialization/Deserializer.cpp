/*!*********************************************************************
\file   Deserializer.cpp
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
#include <Core/Systems/SystemManager/SystemManager.h>
#include <Core/LayerManager/LayerManager.h>
#include <Core/Components/Script.h>
#include <Reflection/ProxyScript.h>

//#define DESERIALIZER_DEBUG

namespace Serialization
{

  void Deserializer::DeserializeAny(rttr::instance inst, std::string const& filename)
  {
    std::ifstream ifs{ filename };
    if (!ifs) {
      Debug::DebugLogger::GetInstance().LogError("[Deserializer] Unable to read " + filename);
      return;
    }

    rapidjson::Document document{};
    rapidjson::IStreamWrapper isw{ ifs };
    if (document.ParseStream(isw).HasParseError())
    {
      ifs.close();
      Debug::DebugLogger::GetInstance().LogError("[Deserializer] Unable to parse " + filename);
      return;
    }

    DeserializeRecursive(inst, document);
  }

  void Deserializer::DeserializeScene(std::vector<Reflection::VariantEntity>& entities,
    Reflection::ObjectFactory::PrefabInstanceContainer& prefabInstances, std::string const& filepath)
  {
    rapidjson::Document document;
    // some initial sanity checks
    // we don't throw here because this is called when the engine
    // first runs, before the catching is established
    if (!ParseJsonIntoDocument(document, filepath)) {
      return;
    }

    if (!document.IsObject() || !document.HasMember(JSON_SCENE_KEY) || !document[JSON_SCENE_KEY].IsArray()) {
      Debug::DebugLogger::GetInstance().LogError("[Deserializer] " + filepath + ": Scene object corrupted!");
#ifdef _DEBUG
      std::cout << filepath + ": Scene object corrupted!" << "\n";
#endif
      return;
    }

    // okay code starts here
    // deserialize all entities
    for (auto const& entity : document[JSON_SCENE_KEY].GetArray())
    {
      if (entity.HasMember(JSON_PREFAB_KEY)) {

        Reflection::PrefabInst inst{};
        if (entity.HasMember(JSON_PFB_POS_KEY)) {
          glm::vec3 pos;
          DeserializeRecursive(pos, entity[JSON_PFB_POS_KEY]);
          inst.mPosition = pos;
        }

        if (!entity.HasMember(JSON_ID_KEY) || !entity.HasMember(JSON_PARENT_KEY) || !entity.HasMember(JSON_CHILD_ENTITIES_KEY)) {
          std::string const msg{ "Reflection::PrefabInst missing members!" };
          Debug::DebugLogger::GetInstance().LogError("[Deserializer] " + msg);
#ifdef _DEBUG
          std::cout << msg << "\n";
#endif
        }
        // deserialize IDs
        inst.mId = static_cast<EntityID>(entity[JSON_ID_KEY].GetUint());
        {
          rapidjson::Value const& parentJson{ entity[JSON_PARENT_KEY] };
          inst.mParent = parentJson.IsNull() ? entt::null : static_cast<EntityID>(parentJson.GetUint());
        }

        {
          rttr::variant childrenVar{ std::vector<ECS::EntityManager::EntityID>() };
          auto seqView{ childrenVar.create_sequential_view() };
          DeserializeSequentialContainer(seqView, entity[JSON_CHILD_ENTITIES_KEY]);
          inst.mChildren = std::move(childrenVar.get_value<std::vector<ECS::EntityManager::EntityID>>());
        }

        DeserializePrefabOverrides(inst.mOverrides, entity[JSON_PREFAB_KEY]);
        Reflection::ObjectFactory::PrefabInstMap& pfbInstances{ prefabInstances[inst.mOverrides.guid] };
        ECS::EntityManager::EntityID const instId{ inst.mId };
        pfbInstances.emplace(instId, std::move(inst));
        continue;
      }

      // check if entity json contains all basic keys
      if (!ScanJsonFileForMembers(entity, filepath, 5,
        JSON_CHILD_ENTITIES_KEY, rapidjson::kArrayType,
        JSON_ID_KEY, rapidjson::kNumberType, JSON_PARENT_KEY, rapidjson::kNumberType,
        JSON_COMPONENTS_KEY, rapidjson::kArrayType, JSON_ENTITY_STATE_KEY, rapidjson::kFalseType))
      {
        continue;
      }

      EntityID entityId{ entity[JSON_ID_KEY].GetUint() };
      EntityID const parentId{ entity[JSON_PARENT_KEY].IsNull() ? entt::null : entity[JSON_PARENT_KEY].GetUint() };
      Reflection::VariantEntity entityVar{ entityId, parentId, entity[JSON_ENTITY_STATE_KEY].GetBool() };  // set parent
      // get child ids
      for (auto const& child : entity[JSON_CHILD_ENTITIES_KEY].GetArray()) {
        entityVar.mChildEntities.emplace_back(EntityID(child.GetUint()));
      }

      // restore components
      std::vector<rttr::variant>& compVector{ entityVar.mComponents };
      for (auto const& elem : entity[JSON_COMPONENTS_KEY].GetArray())
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
            Debug::DebugLogger::GetInstance().LogError("[Deserializer] " + oss.str());
#ifdef _DEBUG
            std::cout << oss.str() << "\n";
#endif
            continue;
          }

          rttr::variant compVar{};
          if (!DeserializeSpecialCases(compVar, compType, compJson)) {
            DeserializeComponent(compVar, compType, compJson);
          }

          compVector.emplace_back(std::move(compVar));
        }
      }

      entities.emplace_back(std::move(entityVar));
    }

    // deserialize layer data
    if (!document.HasMember(JSON_LAYERS_KEY) || !document[JSON_LAYERS_KEY].IsObject()) {
      Debug::DebugLogger::GetInstance().LogError("[Deserializer] " + filepath + ": Unable to find Layer data");
#ifdef _DEBUG
      std::cout << filepath + ": Unable to find Layer data" << "\n";
#endif
      return;
    }

    Layers::LayerManager::LayerData layerData;
    DeserializeRecursive(layerData, document[JSON_LAYERS_KEY]);
    IGE_LAYERMGR.LoadLayerData(std::move(layerData));
  }

  Prefabs::Prefab Deserializer::DeserializePrefabToVariant(std::string const& json)
  {
    rapidjson::Document document{};
    if (!ParseJsonIntoDocument(document, json)) { return {}; }

    if (!ScanJsonFileForMembers(document, json, 2, JSON_COMPONENTS_KEY, rapidjson::kArrayType, JSON_PFB_DATA_KEY, rapidjson::kArrayType)) {
      return {};
    }

    Prefabs::Prefab prefab{};
    prefab.mIsActive = (document.HasMember(JSON_PFB_ACTIVE_KEY) ? document[JSON_PFB_ACTIVE_KEY].GetBool() : true);

    // iterate through component objects in json array
    std::vector<rttr::variant>& compVector{ prefab.mComponents };
    for (auto const& elem : document[JSON_COMPONENTS_KEY].GetArray())
    {
      rapidjson::Value::ConstMemberIterator comp{ elem.MemberBegin() };
      std::string const compName{ comp->name.GetString() };
      rapidjson::Value const& compJson{ comp->value };
      rttr::type compType = rttr::type::get_by_name(compName);

#ifdef DESERIALIZER_DEBUG
      std::cout << "  [P] Deserializing " << compType << "\n";
#endif

      if (!compType.is_valid()) {
        std::ostringstream oss{};
        oss << "Trying to deserialize an invalid component: " << compName;
        Debug::DebugLogger::GetInstance().LogError("[Deserializer] " + oss.str());
#ifdef _DEBUG
        std::cout << oss.str() << "\n";
#endif
        continue;
      }

      rttr::variant compVar{};
      if (!DeserializeSpecialCases(compVar, compType, compJson)) {
        DeserializeComponent(compVar, compType, compJson);
      }

      compVector.emplace_back(compVar);
    }

    for (auto const& elem : document[JSON_PFB_DATA_KEY].GetArray())
    {
      if (!ScanJsonFileForMembers(elem, json, 3, JSON_ID_KEY, rapidjson::kNumberType,
        JSON_COMPONENTS_KEY, rapidjson::kArrayType, JSON_PARENT_KEY, rapidjson::kNumberType)) {
        continue;
      }

      Prefabs::PrefabSubData subObj{ elem[JSON_ID_KEY].GetUint(), elem[JSON_PARENT_KEY].GetUint() };
      subObj.mIsActive = (elem.HasMember(JSON_PFB_ACTIVE_KEY) ? elem[JSON_PFB_ACTIVE_KEY].GetBool() : true);

      for (auto const& component : elem[JSON_COMPONENTS_KEY].GetArray())
      {
        rapidjson::Value::ConstMemberIterator comp{ component.MemberBegin() };
        std::string const compName{ comp->name.GetString() };
        rapidjson::Value const& compJson{ comp->value };
        rttr::type compType = rttr::type::get_by_name(compName);

#ifdef DESERIALIZER_DEBUG
        std::cout << "  [P] Deserializing " << compType << "\n";
#endif

        if (!compType.is_valid())
        {
          std::ostringstream oss{};
          oss << "Trying to deserialize an invalid component: " << compName;
          Debug::DebugLogger::GetInstance().LogError("[Deserializer] " + oss.str());
#ifdef _DEBUG
          std::cout << oss.str() << "\n";
#endif
          continue;
        }

        rttr::variant compVar{};
        DeserializeComponent(compVar, compType, compJson);

        subObj.AddComponent(std::move(compVar));
      }

      prefab.mObjects.emplace_back(std::move(subObj));
    }

    return prefab;
  }

  void Deserializer::DeserializePrefabOverrides(Component::PrefabOverrides& prefabOverride, rapidjson::Value const& json)
  {
    if (!ScanJsonFileForMembers(json, "PrefabOverrides", 4, JSON_GUID_KEY, rapidjson::kObjectType, "modifiedComponents", rapidjson::kArrayType,
      "removedComponents", rapidjson::kArrayType, "subDataId", rapidjson::kNumberType)) {
      return;
    }

    DeserializeRecursive(prefabOverride.guid, json[JSON_GUID_KEY]);
    prefabOverride.subDataId = json["subDataId"].GetUint();

    // deserialize removed components
    {
      rttr::variant var{ std::unordered_set<rttr::type>() };
      auto associativeView{ var.create_associative_view() };
      DeserializeAssociativeContainer(associativeView, json["removedComponents"]);
      prefabOverride.removedComponents = std::move(var.get_value<std::unordered_set<rttr::type>>());
    }

    // deserialize modified components
    rapidjson::Value const& jsonMap{ json["modifiedComponents"] };
    prefabOverride.modifiedComponents.reserve(jsonMap.Size());
    for (rapidjson::SizeType i{}; i < jsonMap.Size(); ++i)
    {
      auto& idxVal{ jsonMap[i] };

      auto keyIter{ idxVal.FindMember(JSON_ASSOCIATIVE_KEY) }, valIter{ idxVal.FindMember(JSON_ASSOCIATIVE_VALUE) };

      if (keyIter == idxVal.MemberEnd() || valIter == idxVal.MemberEnd()) {
        std::string const msg{ "PrefabOverrides::modifiedComponents missing key or value members" };
        Debug::DebugLogger::GetInstance().LogError("[Deserializer] " + msg);
#ifdef _DEBUG
        std::cout << msg << "\n";
#endif
      }

      rttr::type compType{ rttr::type::get_by_name(keyIter->value.GetString()) };
      if (!compType.is_valid())
      {
        std::ostringstream oss{};
        oss << "Trying to deserialize an invalid component: " << compType.get_name().to_string();
        Debug::DebugLogger::GetInstance().LogError("[Deserializer] " + oss.str());
#ifdef _DEBUG
        std::cout << oss.str() << "\n";
#endif
        continue;
      }

      rttr::variant compVar{};
      DeserializeComponent(compVar, compType, valIter->value);
      prefabOverride.modifiedComponents.emplace(std::move(compType), std::move(compVar));
    }
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
              Debug::DebugLogger::GetInstance().LogError("[Deserializer] " + oss.str());
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

      if (args.size() == 1) {
        compVar = compCtr.invoke(args.front());
      }
      else if (args.size() == 2) {
        compVar = compCtr.invoke(args[0], args[1]);
      }
      else if (args.size() == 3) {
        compVar = compCtr.invoke(args[0], args[1], args[2]);
      }
      else {
        compVar = compCtr.invoke_variadic(args);
      }
#ifdef DESERIALIZER_DEBUG
      std::cout << "    Invoked ctor, returning " << compVar.get_type() << "\n";
#endif
    }
  }

  bool Deserializer::DeserializeSpecialCases(rttr::variant& var, rttr::type const& type, rapidjson::Value const& jsonVal)
  {
    if (type == rttr::type::get<Component::Script>()) {
      DeserializeProxyScript(var, jsonVal);
      return true;
    }

    return false;
  }

  void Deserializer::DeserializeRecursive(rttr::instance inst, rapidjson::Value const& jsonObj)
  {
    rttr::instance wrappedInst{ inst.get_type().get_raw_type().is_wrapper() ? inst.get_wrapped_instance() : inst };
    rttr::type baseType{ wrappedInst.get_type() };
    baseType = baseType.is_wrapper() ? baseType.get_wrapped_type().get_raw_type() : baseType.is_pointer() ? baseType.get_raw_type() : baseType;
    for (auto const& prop : baseType.get_properties())
    {
#ifdef DESERIALIZER_DEBUG
      std::cout << "Deserializing property: " << prop.get_name().to_string() << "\n";
#endif
      auto ret{ jsonObj.FindMember(prop.get_name().data()) };
      if (ret == jsonObj.MemberEnd())
      {
        std::string const msg{ "Unable to find property with name: " + prop.get_name().to_string() };
        Debug::DebugLogger::GetInstance().LogError("[Deserializer] " + msg);
#ifdef _DEBUG
        std::cout << msg << "\n";
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
        if (!extractedVal.convert(propType)) {
          std::string const msg{ "Unable to convert element to type " + propType.get_name().to_string() };
          Debug::DebugLogger::GetInstance().LogError("[Deserializer] " + msg);
#ifdef _DEBUG
          std::cout << msg << "\n";
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
      else if (jsonVal.IsUint64())
        return jsonVal.GetUint64();
      else if (jsonVal.IsInt())
        return jsonVal.GetInt();
      else if (jsonVal.IsDouble())
        return jsonVal.GetDouble();
      else if (jsonVal.IsInt64())
        return jsonVal.GetInt64();
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
#ifdef DESERIALIZER_DEBUG
    if (!seqView.is_valid()) {
      std::cout << "Sequential view is not valid!\n";
    }
#endif

#ifdef DESERIALIZER_DEBUG
    std::cout << "    Sequential Container - " << seqView.get_type() << "\n";
#endif
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
        rttr::variant wrappedVal{ wrappedVal.get_type().is_wrapper() ? seqView.get_value(i).extract_wrapped_value() : seqView.get_value(i) };

        DeserializeRecursive(wrappedVal, idxVal);
        seqView.set_value(i, wrappedVal);
      }
      // else its a basic type
      else
      {
        rttr::variant result{ ExtractBasicTypes(idxVal) };
        if (!seqView.set_value(i, result)) {
          rttr::type const seqViewType{ seqView.get_value_type() };
          // temp fix for entt::entity idk man conversion function didnt work
          if (seqViewType == rttr::type::get<entt::entity>()) {
            result = static_cast<entt::entity>(result.get_value<uint32_t>());
            if (seqView.set_value(i, result)) {
              continue;
            }
          }
          // fix for not being able to differentiate between ints and unsigned ints
          else if (idxVal.IsInt()) {
            seqView.set_value(i, idxVal.GetInt());
            continue;
          }
          else if (seqViewType == rttr::type::get<uint64_t>()) {
            seqView.set_value(i, idxVal.GetUint64());
            continue;
          }

          std::string const msg{ "Unable to set sequential view of type " + seqView.get_value_type().get_name().to_string()};
          Debug::DebugLogger::GetInstance().LogError("[Deserializer] " + msg);
#ifdef _DEBUG
          std::cout << msg << "\n";
#endif
        }
      }
    }
  }

  rttr::variant Deserializer::ExtractValue(rapidjson::Value const& jsonVal, rttr::type const& type)
  {
#ifdef DESERIALIZER_DEBUG
    std::cout << "  Extracting value of type " << type << "\n";
#endif
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
      else if (jsonVal.IsArray()) {
        if (type.is_sequential_container()) {
          extractedVal = type.create();
          auto seqView{ extractedVal.create_sequential_view() };
          DeserializeSequentialContainer(seqView, jsonVal);
        }
        else if (type.is_associative_container())
        {
          extractedVal = type.create();
          auto associativeView{ extractedVal.create_associative_view() };
          DeserializeAssociativeContainer(associativeView, jsonVal);
        }
      }
    }

    return extractedVal;
  }

  void Deserializer::DeserializeAssociativeContainer(rttr::variant_associative_view& view, rapidjson::Value const& jsonVal)
  {
#ifdef DESERIALIZER_DEBUG
    std::cout << "    Associative Container - " << view.get_type() << "\n";
#endif
    for (rapidjson::SizeType i{}; i < jsonVal.Size(); ++i)
    {
      auto& idxVal{ jsonVal[i] };
      // if key-value pair
      if (idxVal.IsObject())
      {
        auto keyIter{ idxVal.FindMember("key") }, valIter{ idxVal.FindMember("value") };

        if (keyIter == idxVal.MemberEnd() || valIter == idxVal.MemberEnd()) {
          std::ostringstream oss{};
          oss << "Unable to find key-value pair for element of type " << view.get_key_type().get_name().to_string()
            << "-" << view.get_value_type().get_name().to_string() << " in associative view";
          Debug::DebugLogger::GetInstance().LogError("[Deserializer] " + oss.str());
#ifdef _DEBUG
          std::cout << oss.str() << "\n";
#endif
        }

        auto keyVar{ ExtractValue(keyIter->value, view.get_key_type()) }, valVar{ ExtractValue(valIter->value, view.get_value_type()) };
        if (!keyVar || !valVar) {
          std::ostringstream oss{}, oss2{};
          oss << "Unable to extract key-value pair for element of type " << view.get_key_type().get_name().to_string()
            << "-" << view.get_value_type().get_name().to_string() << " in associative view ";
          oss2 << "Types are " << keyVar.get_type().get_name().to_string() << ", " << valVar.get_type().get_name().to_string();
          Debug::DebugLogger::GetInstance().LogError("[Deserializer] " + oss.str());
#ifdef _DEBUG
          std::cout << oss.str() << "\n" << oss2.str() << '\n';
#endif
        }
        else
        {
          auto result{ view.insert(keyVar, valVar) };
          if (!result.second)
          {
            // temp fix for entt::entity idk man conversion function didnt work
            if (view.get_value_type() == rttr::type::get<entt::entity>()) {
              valVar = static_cast<entt::entity>(valVar.get_value<uint32_t>());
              if (view.insert(keyVar, valVar).second) {
                continue;
              }
            }
            std::ostringstream oss{};
            oss << "Unable to insert key-value pair for element of type " << view.get_key_type().get_name().to_string()
              << "-" << view.get_value_type().get_name().to_string();
            Debug::DebugLogger::GetInstance().LogError("[Deserializer] " + oss.str());
#ifdef _DEBUG
            std::cout << oss.str() << "\n";
            std::cout << "Types are " << keyVar.get_type().get_name().to_string() << " and " << valVar.get_type().get_name().to_string() << "\n";
#endif
          }
        }
      }
      // else if key-only
      else
      {
        rttr::variant extractedVal{ ExtractBasicTypes(idxVal) };
        if (!extractedVal || !extractedVal.convert(view.get_key_type())) {
          std::string const msg{ "Unable to extract key-only type of " + view.get_key_type().get_name().to_string() };
          Debug::DebugLogger::GetInstance().LogError("[Deserializer] " + msg);
#ifdef _DEBUG
          std::cout << msg << "\n";
#endif
        }

        auto result{ view.insert(extractedVal) };
        if (!result.second) {
          std::string const msg{ "Unable to insert key-only type of " + view.get_key_type().get_name().to_string() };
          Debug::DebugLogger::GetInstance().LogError("[Deserializer] " + msg);
#ifdef _DEBUG
          std::cout << msg  << "\n";
#endif
        }
      }
    }
  }

#pragma region ScriptStuff
  void Deserializer::DeserializeProxyScript(rttr::variant& var, rapidjson::Value const& jsonVal) {

    if (!jsonVal.HasMember(JSON_SCRIPT_LIST_KEY) || !jsonVal[JSON_SCRIPT_LIST_KEY].IsArray()) {
      Debug::DebugLogger::GetInstance().LogError("[Deserializer] Script component missing \"" JSON_SCRIPT_LIST_KEY "\" field");
      var = {}; return;
    }

    Reflection::ProxyScriptComponent proxyScriptComp{};
    auto const& jsonScriptArr{ jsonVal[JSON_SCRIPT_LIST_KEY].GetArray() };
    proxyScriptComp.proxyScriptList.reserve(jsonScriptArr.Size());
    for (rapidjson::Value const& elem : jsonScriptArr) {
      // check if fields exist in json file
      if (!ScanJsonFileForMembers(elem, "Script Instance", 2, JSON_SCRIPT_NAME_KEY, rapidjson::kStringType,
        JSON_SCRIPT_FIELD_LIST_KEY, rapidjson::kArrayType)) {
        continue;
      }

      Reflection::ProxyScript scriptInst{ elem[JSON_SCRIPT_NAME_KEY].GetString() };
      auto const& fieldListJson{ elem[JSON_SCRIPT_FIELD_LIST_KEY].GetArray() };
      scriptInst.scriptFieldProxyList.reserve(fieldListJson.Size());

      for (rapidjson::Value const& fieldInst : fieldListJson) {
        rttr::variant scriptFIList{ DeserializeDataMemberInstance(fieldInst) };
        if (!scriptFIList.is_valid()) { continue; }

        scriptInst.scriptFieldProxyList.emplace_back(std::move(scriptFIList));
      }
      proxyScriptComp.proxyScriptList.emplace_back(std::move(scriptInst));
    }

    var = std::move(proxyScriptComp);
  }

  rttr::variant Deserializer::DeserializeDataMemberInstance(rapidjson::Value const& jsonVal) {
    // maybe should just call ScanJsonFileForMembers()
    if (!jsonVal.HasMember(JSON_SCRIPT_DMI_TYPE_KEY) || !jsonVal.HasMember(JSON_SCRIPT_DMI_DATA_KEY)
      || !jsonVal.HasMember(JSON_SCRIPT_DMI_SF_KEY)) {
      Debug::DebugLogger::GetInstance().LogError("[Deserializer] Data member inst missing members!");
      return {};
    }

    rttr::type const scriptFIType{ rttr::type::get_by_name(jsonVal[JSON_SCRIPT_DMI_TYPE_KEY].GetString()) };
    rttr::variant scriptFIList{ scriptFIType.create() };
#ifdef DESERIALIZER_DEBUG
    std::cout << "    Processing DataMemberInstance of type " << jsonVal[JSON_SCRIPT_DMI_TYPE_KEY].GetString() << "\n";
    if (!scriptFIList.is_valid()) {
      std::cout << "    Unable to create variant!\n";
    }
#endif

    //  if it is a multi-layered DataMemberInstance, recursively extract each layer
    if (scriptFIType == rttr::type::get<Mono::DataMemberInstance<Mono::ScriptInstance>>()) {
      // set the data property
      rttr::variant data{ DeserializeScriptInstance(jsonVal[JSON_SCRIPT_DMI_DATA_KEY]) };
      if (!scriptFIType.set_property_value(JSON_SCRIPT_DMI_DATA_KEY, scriptFIList, data)) {
        std::ostringstream oss{};
        oss << "[Deserializer] Unable to set " JSON_SCRIPT_DMI_DATA_KEY " of DataMemberInstance : "
          << jsonVal[JSON_SCRIPT_DMI_SF_KEY].GetObject()["fieldName"].GetString();
        IGE_DBGLOGGER.LogError(oss.str());
#ifdef DESERIALIZER_DEBUG
        std::cout << oss.str() << "\n";
        std::cout << "  Argument was " << data.get_type() << ", expected "
          << scriptFIType.get_property(JSON_SCRIPT_DMI_DATA_KEY).get_type() << "\n";
#endif
      }

      // set script field property
      Mono::ScriptFieldInfo sfInfo{};
      DeserializeRecursive(sfInfo, jsonVal[JSON_SCRIPT_DMI_SF_KEY]);
      if (!scriptFIType.set_property_value(JSON_SCRIPT_DMI_SF_KEY, scriptFIList, sfInfo)) {
        std::ostringstream oss{};
        oss << "[Deserializer] Unable to set " JSON_SCRIPT_DMI_SF_KEY " of DataMemberInstance: "
          << jsonVal[JSON_SCRIPT_DMI_SF_KEY].GetObject()["fieldName"].GetString();
        IGE_DBGLOGGER.LogError(oss.str());
#ifdef DESERIALIZER_DEBUG
        std::cout << oss.str() << "\n";
        std::cout << "  Argument was " << rttr::type::get(sfInfo) << ", expected "
          << scriptFIType.get_property(JSON_SCRIPT_DMI_SF_KEY).get_type() << "\n";
#endif
      }
    }
    //  else deserialize normally
    else {
      DeserializeRecursive(scriptFIList, jsonVal);
    }

    return scriptFIList;
  }

  Mono::ScriptInstance Deserializer::DeserializeScriptInstance(rapidjson::Value const& jsonVal) {
    // check if fields exist in json file
    if (!ScanJsonFileForMembers(jsonVal, "Script Instance", 2, JSON_SCRIPT_NAME_KEY, rapidjson::kStringType,
      JSON_SCRIPT_FIELD_LIST_KEY, rapidjson::kArrayType)) {
      return {};
    }

#ifdef DESERIALIZER_DEBUG
    std::cout << "  Deserializing ScriptInstance: \"" << jsonVal[JSON_SCRIPT_NAME_KEY].GetString() << "\"\n";
#endif

    Mono::ScriptInstance scriptInst{ jsonVal[JSON_SCRIPT_NAME_KEY].GetString() };

    auto const& fieldListJson{ jsonVal[JSON_SCRIPT_FIELD_LIST_KEY].GetArray() };
    std::vector<rttr::variant> sfInstList{};
    sfInstList.reserve(fieldListJson.Size());

    for (rapidjson::Value const& fieldInst : fieldListJson) {
      rttr::variant scriptFIList{ DeserializeDataMemberInstance(fieldInst) };
      if (!scriptFIList.is_valid()) { continue; }

      sfInstList.emplace_back(std::move(scriptFIList));
    }
    scriptInst.SetAllFields(sfInstList);

    return scriptInst;
  }
#pragma endregion

  bool Deserializer::ParseJsonIntoDocument(rapidjson::Document& document, std::string const& filepath)
  {
    std::ifstream ifs{ filepath };
    if (!ifs) {
      Debug::DebugLogger::GetInstance().LogError("[Deserializer] Unable to read " + filepath);
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
      ifs.close();
      Debug::DebugLogger::GetInstance().LogError("[Deserializer] Unable to parse " + filepath);
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
            Debug::DebugLogger::GetInstance().LogError("[Deserializer] " + oss.str());
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
              Debug::DebugLogger::GetInstance().LogError("[Deserializer] " + oss.str());
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
            Debug::DebugLogger::GetInstance().LogError("[Deserializer] " + oss.str());
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
          Debug::DebugLogger::GetInstance().LogError("[Deserializer] " + oss.str());
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
            Debug::DebugLogger::GetInstance().LogError("[Deserializer] " + oss.str());
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
          Debug::DebugLogger::GetInstance().LogError("[Deserializer] " + oss.str());
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
