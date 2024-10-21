/*!*********************************************************************
\file   Serializer.cpp
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
#include <pch.h>
#include "Serializer.h"
#include <Serialization/JsonKeys.h>
#include <Core/Components/Components.h>
#include <fstream>
#include <Reflection/ObjectFactory.h>
#include <Prefabs/PrefabManager.h>

namespace Helper {
  template <typename T>
  bool IsType(rttr::type const& t) { return t == rttr::type::get<T>(); }
}

namespace Serialization
{

//#ifndef IMGUI_DISABLE
  void Serializer::SerializePrefab(Prefabs::Prefab const& prefab, std::string const& filePath)
  {
    std::ofstream ofs{ filePath };
    if (!ofs) {
      Debug::DebugLogger::GetInstance().LogCritical("Unable to serialize prefab into " + filePath);
#ifdef _DEBUG
      std::cout << "Unable to serialize prefab into " + filePath << "\n";
#endif
      return;
    }

    rapidjson::OStreamWrapper osw{ ofs };
    WriterType writer{ osw };
    writer.StartObject();

    // serialize the base layer of the prefab
    writer.Key(JSON_PFB_NAME_KEY); writer.String(prefab.mName.c_str());
    writer.Key(JSON_PFB_ACTIVE_KEY); writer.Bool(true);

    writer.Key(JSON_COMPONENTS_KEY);
    SerializeVariantComponents(prefab.mComponents, writer);

    // serialize nested components if prefab has multiple layers
    writer.Key(JSON_PFB_DATA_KEY);
    writer.StartArray();
    for (Prefabs::PrefabSubData const& obj : prefab.mObjects)
    {
      writer.StartObject();
      
      writer.Key(JSON_ID_KEY); writer.Uint(obj.mId);
      writer.Key(JSON_PFB_ACTIVE_KEY); writer.Bool(true);
      writer.Key(JSON_PARENT_KEY);
      if (obj.mParent == entt::null) {
        writer.Null();
      }
      else {
        writer.Uint(obj.mParent);
      }
      
      writer.Key(JSON_COMPONENTS_KEY);
      SerializeVariantComponents(obj.mComponents, writer);

      writer.EndObject();
    }
    writer.EndArray();

    writer.EndObject();
    ofs.close();
  }
//#endif

  void Serializer::SerializeAny(rttr::instance const& obj, std::string const& filename)
  {
    std::ofstream ofs{ filename };
    if (!ofs)
    {
      Debug::DebugLogger::GetInstance().LogError("[Serializer] Unable to create file: " + filename);
#ifdef _DEBUG
      std::cout << "Unable to open file " << filename << "\n";
#endif
      return;
    }

    rapidjson::OStreamWrapper osw{ ofs };
    WriterType writer{ osw };
    SerializeClassTypes(obj, writer);
    ofs.close();
  }

  void Serializer::SerializeScene(std::string const& filePath)
  {
    std::ofstream ofs{ filePath };
    if (!ofs) {
      Debug::DebugLogger::GetInstance().LogError("[Serializer] Unable to create scene file: " + filePath);
    }
    rapidjson::OStreamWrapper osw{ ofs };
    WriterType writer{ osw };

    EntityList entityList{ GetSortedEntities() };

    writer.StartArray();
    while (!entityList.empty()) {
      SerializeEntity(entityList.top(), writer);
      entityList.pop();
    }
    writer.EndArray();

    // clean up
    ofs.close();
  }

  void Serializer::SerializeEntity(ECS::Entity const& entity, WriterType& writer)
  {
    writer.StartObject();
    ECS::EntityManager& entityMan{ ECS::EntityManager::GetInstance() };

    // serialize entity id
    writer.Key(JSON_ID_KEY);
    writer.Uint(entity.GetEntityID());

    // serialize parent id
    writer.Key(JSON_PARENT_KEY);
    if (entityMan.HasParent(entity)) {
      writer.Uint(entityMan.GetParentEntity(entity).GetEntityID());
    }
    else {
      writer.Null();
    }

    // serialize child entities
    writer.Key(JSON_CHILD_ENTITIES_KEY);
    writer.StartArray();
    if (entityMan.HasChild(entity)) {
      for (auto const& child : entityMan.GetChildEntity(entity)) {
        writer.Uint(child.GetEntityID());
      }
    }
    writer.EndArray();

    // if its a prefab instance, only serialize the overrides
    if (entity.HasComponent<Component::PrefabOverrides>()) {
      Component::PrefabOverrides const& overrides{ entity.GetComponent<Component::PrefabOverrides>() };

      // if position exists, serialize it
      if (overrides.subDataId == Prefabs::PrefabSubData::BasePrefabId && !overrides.IsComponentModified(rttr::type::get<Component::Transform>())) {
        writer.Key(JSON_PFB_POS_KEY);
        Debug::DebugLogger::GetInstance().LogInfo(std::to_string(entity.GetComponent<Component::Transform>().worldPos.x));
        SerializeRecursive(entity.GetComponent<Component::Transform>().worldPos, writer);
      }

      // serialize the components
      writer.Key(JSON_PREFAB_KEY);
      SerializeRecursive(overrides, writer);
      writer.EndObject();
      return;
    }

    // if not, serialize the entity as per normal
    // 
    // serialize state
    writer.Key(JSON_ENTITY_STATE_KEY);
    writer.Bool(true);// entityMan.GetIsActiveEntity(entity));

    writer.Key(JSON_COMPONENTS_KEY);
    std::vector<rttr::variant> const components{ Reflection::ObjectFactory::GetInstance().GetEntityComponents(entity) };
    SerializeVariantComponents(components, writer);
    writer.EndObject();
  }

  void Serializer::SerializeVariantComponents(std::vector<rttr::variant> const& components, WriterType& writer)
  {
    writer.StartArray();
    // for each component, extract the string of the class and serialize
    for (rttr::variant const& comp : components)
    {
      writer.StartObject();

      rttr::type compType{ comp.get_type() };
      // get underlying type if it's wrapped in a pointer
      compType = compType.is_wrapper() ? compType.get_wrapped_type().get_raw_type() : compType.is_pointer() ? compType.get_raw_type() : compType;

      writer.Key(compType.get_name().to_string().c_str());
      //if (!SerializeSpecialCases(comp, compType, writer)) {
        SerializeClassTypes(comp, writer);
      //}
      writer.EndObject();
    }
    writer.EndArray();
  }

  bool Serializer::SerializeSpecialCases(rttr::instance const& obj, rttr::type const& type, WriterType& writer)
  {
    return false;
  }

  void Serializer::SerializeClassTypes(rttr::instance const& obj, WriterType& writer)
  {
    writer.StartObject();

    rttr::instance wrappedObj{ obj.get_type().get_raw_type().is_wrapper() ? obj.get_wrapped_instance() : obj };

    auto const properties{ wrappedObj.get_type().get_properties() };
    for (auto const& property : properties)
    {
      //if (property.get_metadata("NO_SERIALIZE")) { continue; }
      rttr::variant propVal{ property.get_value(wrappedObj) };
      if (!propVal) {
        std::ostringstream oss{};
        oss << "Unable to serialize property " << property.get_name().to_string() << " of type " << property.get_type().get_name().to_string();
        Debug::DebugLogger::GetInstance().LogError("[Serializer] " + oss.str());
#ifdef _DEBUG
        std::cout << oss.str() << "\n";
#endif
        continue;
      }

      std::string const name{ property.get_name().to_string() };
      writer.String(name.c_str(), static_cast<rapidjson::SizeType>(name.length()), false);
      if (!SerializeRecursive(propVal, writer))
      {
        std::ostringstream oss{};
        oss << "Unable to serialize property " << name << " of type " << property.get_type().get_name().to_string();
        Debug::DebugLogger::GetInstance().LogError("[Serializer] " + oss.str());
#ifdef _DEBUG
        std::cout << oss.str() << "\n";
#endif
      }
    }

    writer.EndObject();
  }

  bool Serializer::WriteBasicTypes(rttr::type const& type, rttr::variant const& var, WriterType& writer)
  {
    // if basic C type
    if (type.is_arithmetic())
    {
      if (Helper::IsType<float>(type)) { writer.Double(var.to_float()); }
      if (Helper::IsType<double>(type)) { writer.Double(var.to_double()); }
      else if (Helper::IsType<int64_t>(type)) { writer.Int64(var.to_int64()); }
      else if (Helper::IsType<uint64_t>(type)) { writer.Uint64(var.to_uint64()); }
      else if (Helper::IsType<bool>(type)) { writer.Bool(var.to_bool()); }
      else if (Helper::IsType<char>(type)) { writer.Bool(var.to_bool()); }
      else if (Helper::IsType<int8_t>(type)) { writer.Int(var.to_int8()); }
      else if (Helper::IsType<int16_t>(type)) { writer.Int(var.to_int16()); }
      else if (Helper::IsType<int32_t>(type)) { writer.Int(var.to_int32()); }
      else if (Helper::IsType<uint16_t>(type)) { writer.Uint(var.to_uint8()); }
      else if (Helper::IsType<uint16_t>(type)) { writer.Uint(var.to_uint16()); }
      else if (Helper::IsType<uint32_t>(type)) { writer.Uint(var.to_uint32()); }

      return true;
    }
    else if (type == rttr::type::get<std::string>() || type == rttr::type::get<const char*>())
    {
      writer.String(var.to_string().c_str());

      return true;
    }
    else if (type.is_enumeration())
    {
      bool result;
      std::string str{ var.to_string(&result) };
      if (result) {
        writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.length()), false);
      }
      else
      {
        uint64_t val{ var.to_uint64(&result) };
        if (result) {
          writer.Uint64(val);
        }
        else {
          writer.Null();
        }
      }

      return true;
    }

    return false;
  }

  void Serializer::WriteSequentialContainer(rttr::variant_sequential_view const& seqView, WriterType& writer)
  {
    writer.StartArray();

    for (auto const& elem : seqView)
    {
      // if elem is another sequential container, call this function again
      if (elem.is_sequential_container()) {
        WriteSequentialContainer(elem.create_sequential_view(), writer);
      }
      else
      {
        rttr::variant wrappedVar{ elem.extract_wrapped_value() };
        rttr::type const valType{ wrappedVar.get_type() };
        if (!WriteBasicTypes(valType, wrappedVar, writer))
        {
          SerializeClassTypes(wrappedVar, writer);
        }
      }
    }

    writer.EndArray();
  }

  bool Serializer::SerializeRecursive(rttr::variant const& var, WriterType& writer)
  {
    bool const isWrapper{ var.get_type().is_wrapper() };
    rttr::type const type{ isWrapper ? var.get_type().get_wrapped_type().get_raw_type() :
          var.get_type().is_pointer() ? var.get_type().get_raw_type() : var.get_type() };

    if (WriteBasicTypes(type, isWrapper ? var.extract_wrapped_value() : var, writer)) {

    }
    else if (var.is_sequential_container())
    {
      WriteSequentialContainer(var.create_sequential_view(), writer);
    }
    else if (var.is_associative_container())
    {
      WriteAssociativeContainer(var.create_associative_view(), writer);
    }
    else
    {
      auto properties{ type.get_properties() };
      if (!properties.empty())
      {
        SerializeClassTypes(var, writer);
      }
      else if (type == rttr::type::get<rttr::type>()) {
        bool ok;
        writer.String(var.convert<std::string>(&ok).c_str());
        return true;
      }
      else {
        std::string const msg{ "Unable to write variant of type " + (isWrapper ? type.get_name().to_string() : type.get_name().to_string()) };
        Debug::DebugLogger::GetInstance().LogError("[Serializer] " + msg);
#ifdef _DEBUG
        std::cout << msg << "\n";
#endif

        return false;
      }
    }

    return true;
  }

  void Serializer::WriteAssociativeContainer(rttr::variant_associative_view const& view, WriterType& writer)
  {
    writer.StartArray();

    if (view.is_key_only_type()) {
      for (auto const& elem : view) {
        SerializeRecursive(elem.first, writer);
      }
    }
    else {
      for (auto const& elem : view)
      {
        writer.StartObject();
        writer.String("key", 3, false);
        SerializeRecursive(elem.first, writer);

        writer.String("value", 5, false);
        SerializeRecursive(elem.second.get_type().is_wrapper() ? elem.second.extract_wrapped_value() : elem.second, writer);

        writer.EndObject();
      }
    }

    writer.EndArray();
  }

  Serializer::EntityList Serializer::GetSortedEntities() {
    auto const& entityList{ ECS::EntityManager::GetInstance().GetAllEntities() };
    EntityList entityPQ{};

    for (ECS::Entity e : entityList) { entityPQ.emplace(e); }

    return entityPQ;
  }

} // namespace Serialization
