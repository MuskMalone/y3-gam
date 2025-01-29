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
#include <fstream>
#include <rapidjson/Writer.h>
#include <rapidjson/PrettyWriter.h>
#include <rapidjson/filewritestream.h>

#include <Serialization/JsonKeys.h>
#include <Serialization/FILEWrapper.h>
#include <Serialization/PfbOverridesData.h>
#include <Core/Components/Components.h>
#include <Core/Entity.h>
#include <Animation/Keyframe.h>

#include <Prefabs/PrefabManager.h>
#include <Reflection/ObjectFactory.h>
#include <Core/LayerManager/LayerManager.h>
#include <Scripting/ScriptManager.h>
#include <Scripting/ScriptInstance.h>

namespace Helper {
  template <typename T>
  bool IsType(rttr::type const& t) { return t == rttr::type::get<T>(); }
}

namespace {
  using StreamType = rapidjson::FileWriteStream;
  using PrettyWriterType = rapidjson::PrettyWriter<StreamType>;
  using CompactWriterType = rapidjson::Writer<StreamType>;
  using MonoObjectVector = std::vector<MonoObject*>;

  static rttr::type const sScriptCompType = rttr::type::get<Component::Script>();
  static rttr::type const sMonoObjectVecType = rttr::type::get<MonoObjectVector>();
  static constexpr unsigned sBufferSize = 65536;

  // had to decltype a lambda LOL
  using EntityList = std::priority_queue < ECS::Entity, std::vector<ECS::Entity>,
    decltype([](ECS::Entity const& lhs, ECS::Entity const& rhs) { return lhs.GetRawEnttEntityID() > rhs.GetRawEnttEntityID(); }) > ;

  template <typename WriterType>
  void SerializeSceneInternal(std::string const& filePath);

  template <typename WriterType>
  void SerializePrefabInternal(Prefabs::Prefab const& prefab, std::string const& filePath);

  /*!*********************************************************************
  \brief
    Helper function to serialize an entity's properties
  \param entity
    The entity to serialize
  \param writer
    The writer to write to
  ************************************************************************/
  template <typename WriterType>
  void SerializeEntity(ECS::Entity const& entity, WriterType& writer);

  /*!*********************************************************************
  \brief
    Helper function to serialize a vector of components of an entity
  \param components
    The vector of components
  \param writer
    The writer to write to
  ************************************************************************/
  template <typename WriterType>
  void SerializeVariantComponents(std::vector<rttr::variant> const& components, WriterType& writer);

  /*!*********************************************************************
  \brief
    Serializes a class by iterating through its properties
  \param object
    The object to serialize
  \param writer
    The writer to write to
  ************************************************************************/
  template <typename WriterType>
  void SerializeClassTypes(rttr::instance const& obj, WriterType& writer);

  template <typename WriterType>
  void SerializeScriptClassTypes(rttr::instance const& obj, WriterType& writer);
  template <typename WriterType>
  bool SerializeScriptRecursive(rttr::variant const& var, WriterType& writer);
  template <typename WriterType>
  void WriteScriptSequentialContainer(rttr::variant_sequential_view const& seqView, WriterType& writer);

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
  //template <typename WriterType>
  //static bool SerializeSpecialCases(rttr::instance const& obj, rttr::type const& type, WriterType& writer);

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
  template <typename WriterType>
  bool WriteBasicTypes(rttr::type const& type, rttr::variant const& var, WriterType& writer);

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
  template <typename WriterType>
  void WriteSequentialContainer(rttr::variant_sequential_view const& seqView, WriterType& writer);

  /*!*********************************************************************
  \brief
    Retrieves the entities from the ECS and sorts them based on the pq
    defined in EntityList
  \return
    The list of sorted entities
  ************************************************************************/
  EntityList GetSortedEntities();    // @TODO: May need to get entities from particular scene next time
                                     //        instead of ECS

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
  template <typename WriterType>
  bool SerializeRecursive(rttr::variant const& var, WriterType& writer);

  template <typename WriterType>
  bool SerializeCustomTypes(rttr::variant const& var, rttr::type const& type, WriterType& writer);

  /*!*********************************************************************
  \brief
    Serializes an rttr::variant containing an associative container type
    (such as std::map)
  \param view
    The associative view of the container
  \param writer
    The writer to write to
  ************************************************************************/
  template <typename WriterType>
  void WriteAssociativeContainer(rttr::variant_associative_view const& view, WriterType& writer);

  template<typename WriterType>
  void SerializeNextNodes(Anim::Node const& node, WriterType& writer);
}

namespace Serialization
{
  void Serializer::SerializePrefab(Prefabs::Prefab const& prefab, std::string const& filePath, FileFormat format) {
    if (format == FileFormat::PRETTY) {
      SerializePrefabInternal<PrettyWriterType>(prefab, filePath);
      return;
    }

    SerializePrefabInternal<CompactWriterType>(prefab, filePath);
  }

  void Serializer::SerializeAny(rttr::instance const& obj, std::string const& filePath, FileFormat format)
  {
    FILEWrapper fileWrapper{ filePath.c_str(), "w" };
    if (!fileWrapper) {
      Debug::DebugLogger::GetInstance().LogError("[Serializer] Unable to create file: " + filePath);
#ifdef _DEBUG
      std::cout << "Unable to open file " << filePath << "\n";
#endif
      return;
    }

    std::vector<char> buffer(sBufferSize);
    StreamType outStream{ fileWrapper.GetFILE(), buffer.data(), sBufferSize };
    if (format == FileFormat::PRETTY) {
      PrettyWriterType writer{ outStream };
      SerializeClassTypes(obj, writer);
    }
    else {
      CompactWriterType writer{ outStream };
      SerializeClassTypes(obj, writer);
    }
  }

  void Serializer::SerializeScene(std::string const& filePath, FileFormat format) {
    if (format == FileFormat::PRETTY) {
      SerializeSceneInternal<PrettyWriterType>(filePath);
      return;
    }

    SerializeSceneInternal<CompactWriterType>(filePath);
  }

  void Serializer::SerializeAnimationData(Anim::AnimationData const& animData, std::string const& filePath) {
    Serialization::FILEWrapper fileWrapper{ filePath.c_str(), "w" };
    if (!fileWrapper) {
      Debug::DebugLogger::GetInstance().LogError("[Serializer] Unable to create AnimationData file: " + filePath);
      return;
    }

    std::vector<char> buffer(sBufferSize);
    StreamType outStream{ fileWrapper.GetFILE(), buffer.data(), sBufferSize };
    PrettyWriterType writer{ outStream };
    
    writer.StartObject();

    writer.Key("rootKeyframe");
    writer.StartObject();

    writer.Key("startPos");
    SerializeRecursive(animData.rootKeyframe.startPos, writer);
    writer.Key("startRot");
    SerializeRecursive(animData.rootKeyframe.startRot, writer);
    writer.Key("startScale");
    SerializeRecursive(animData.rootKeyframe.startScale, writer);

    writer.Key("nextNodes");
    // we'll use the ptr address as the node ID
    writer.StartArray();
    for (Anim::Node const& node : animData.rootKeyframe.nextNodes) {
      writer.Uint64((uintptr_t)node.get());
    }
    writer.EndArray();
    writer.EndObject();

    writer.Key("keyframes");
    writer.StartArray();
    for (Anim::Node const& node : animData.rootKeyframe.nextNodes) {
      SerializeNextNodes(node, writer);
    }
    writer.EndArray();

    writer.EndObject();
  }
} // namespace Serialization

namespace {
  template <typename WriterType>
  void SerializeSceneInternal(std::string const& filePath) {
    Serialization::FILEWrapper fileWrapper{ filePath.c_str(), "w" };
    if (!fileWrapper) {
      Debug::DebugLogger::GetInstance().LogError("[Serializer] Unable to create scene file: " + filePath);
      return;
    }

    std::vector<char> buffer(sBufferSize);
    StreamType outStream{ fileWrapper.GetFILE(), buffer.data(), sBufferSize };

    WriterType writer{ outStream };

    EntityList entityList{ GetSortedEntities() };

    // serialize entities
    writer.StartObject();

    writer.Key(JSON_SCENE_KEY);
    writer.StartArray();
    while (!entityList.empty()) {
      SerializeEntity<WriterType>(entityList.top(), writer);
      entityList.pop();
    }
    writer.EndArray();

    // serialize layer data
    writer.Key(JSON_LAYERS_KEY);
    SerializeClassTypes<WriterType>(IGE_LAYERMGR.GetLayerData(), writer);

    // serialize global light properties
    writer.Key(JSON_GLOBAL_PROPS_KEY);
    SerializeClassTypes<WriterType>(Component::Light::sGlobalProps, writer);

    writer.EndObject();
  }

  template <typename WriterType>
  void SerializePrefabInternal(Prefabs::Prefab const& prefab, std::string const& filePath) {
    Serialization::FILEWrapper fileWrapper{ filePath.c_str(), "w" };
    if (!fileWrapper) {
      Debug::DebugLogger::GetInstance().LogCritical("Unable to serialize prefab into " + filePath);
#ifdef _DEBUG
      std::cout << "Unable to serialize prefab into " + filePath << "\n";
#endif
      return;
    }

    std::vector<char> buffer(sBufferSize);
    StreamType outStream{ fileWrapper.GetFILE(), buffer.data(), sBufferSize };
    WriterType writer{ outStream };
    writer.StartObject();

    // serialize the base layer of the prefab
    writer.Key(JSON_PFB_NAME_KEY); writer.String(prefab.mName.c_str());

    // serialize all layers of the prefab
    writer.Key(JSON_PFB_DATA_KEY);
    writer.StartArray();
    for (Prefabs::PrefabSubData const& obj : prefab.mObjects)
    {
      writer.StartObject();

      writer.Key(JSON_ID_KEY); writer.Uint(obj.mId);
      writer.Key(JSON_PARENT_KEY);
      if (obj.mParent == Prefabs::PrefabSubData::InvalidId) {
        writer.Null();
      }
      else {
        writer.Uint(obj.mParent);
      }

      writer.Key(JSON_COMPONENTS_KEY);
      SerializeVariantComponents<WriterType>(obj.mComponents, writer);

      writer.EndObject();
    }
    writer.EndArray();

    writer.EndObject();
  }

  template <typename WriterType>
  void SerializeEntity(ECS::Entity const& entity, WriterType& writer)
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
        SerializeRecursive<WriterType>(entity.GetComponent<Component::Transform>().position, writer);
      }

      // serialize the components
      writer.Key(JSON_PREFAB_KEY);
      Serialization::PfbOverridesData overridesData{ entity.GetComponent<Component::PrefabOverrides>(), entity };
      SerializeRecursive<WriterType>(overridesData, writer);
      writer.EndObject();
      return;
    }

    // if not, serialize the entity as per normal
    writer.Key(JSON_COMPONENTS_KEY);
    std::vector<rttr::variant> const components{ Reflection::ObjectFactory::GetInstance().GetEntityComponents(entity) };
    SerializeVariantComponents<WriterType>(components, writer);
    writer.EndObject();
  }

  template <typename WriterType>
  void SerializeVariantComponents(std::vector<rttr::variant> const& components, WriterType& writer)
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

      // handle scripts separately
      if (compType == sScriptCompType) {
        SerializeScriptClassTypes<WriterType>(comp, writer);
      }
      else {
        SerializeClassTypes<WriterType>(comp, writer);
      }

      writer.EndObject();
    }
    writer.EndArray();
  }

  //template <typename WriterType>
  //bool Serializer::SerializeSpecialCases(rttr::instance const& obj, rttr::type const& type, WriterType& writer)
  //{
  //  return false;
  //}

  template <typename WriterType>
  void SerializeClassTypes(rttr::instance const& obj, WriterType& writer)
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
      if (!SerializeRecursive<WriterType>(propVal, writer))
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

  template <typename WriterType>
  bool WriteBasicTypes(rttr::type const& type, rttr::variant const& var, WriterType& writer)
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
      else if (Helper::IsType<uint8_t>(type)) { writer.Uint(var.to_uint8()); }
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

  template <typename WriterType>
  void WriteSequentialContainer(rttr::variant_sequential_view const& seqView, WriterType& writer)
  {
    writer.StartArray();

    for (auto const& elem : seqView)
    {
      // if elem is another sequential container, call this function again
      if (elem.is_sequential_container()) {
        WriteSequentialContainer<WriterType>(elem.create_sequential_view(), writer);
      }
      else
      {
        rttr::variant wrappedVar{ elem.extract_wrapped_value() };
        rttr::type const valType{ wrappedVar.get_type() };
        if (!WriteBasicTypes<WriterType>(valType, wrappedVar, writer))
        {
          SerializeClassTypes<WriterType>(wrappedVar, writer);
        }
      }
    }

    writer.EndArray();
  }

  template <typename WriterType>
  bool SerializeRecursive(rttr::variant const& var, WriterType& writer)
  {
    bool const isWrapper{ var.get_type().is_wrapper() };
    rttr::type const type{ isWrapper ? var.get_type().get_wrapped_type().get_raw_type() :
          var.get_type().is_pointer() ? var.get_type().get_raw_type() : var.get_type() };

    if (WriteBasicTypes<WriterType>(type, isWrapper ? var.extract_wrapped_value() : var, writer)) {

    }
    else if (var.is_sequential_container())
    {
      WriteSequentialContainer<WriterType>(var.create_sequential_view(), writer);
    }
    else if (var.is_associative_container())
    {
      WriteAssociativeContainer<WriterType>(var.create_associative_view(), writer);
    }
    else
    {
      if (SerializeCustomTypes(var, type, writer)) {
        return true;
      }
      else if (type.get_properties().empty()) {
        std::string const msg{ "Unable to write variant of type " + (isWrapper ? type.get_name().to_string() : type.get_name().to_string()) };
        Debug::DebugLogger::GetInstance().LogError("[Serializer] " + msg);
#ifdef _DEBUG
        std::cout << msg << "\n";
#endif

        return false;
      }
      
      SerializeClassTypes<WriterType>(var, writer);
    }

    return true;
  }

  template <typename WriterType>
  void WriteAssociativeContainer(rttr::variant_associative_view const& view, WriterType& writer)
  {
    writer.StartArray();

    if (view.is_key_only_type()) {
      for (auto const& elem : view) {
        SerializeRecursive<WriterType>(elem.first, writer);
      }
    }
    else {
      for (auto const& elem : view)
      {
        writer.StartObject();
        writer.String("key", 3, false);
        SerializeRecursive<WriterType>(elem.first, writer);

        writer.String("value", 5, false);
        SerializeRecursive<WriterType>(elem.second.get_type().is_wrapper() ? elem.second.extract_wrapped_value() : elem.second, writer);

        writer.EndObject();
      }
    }

    writer.EndArray();
  }

  template <typename WriterType>
  bool SerializeCustomTypes(rttr::variant const& var, rttr::type const& type, WriterType & writer) {
    if (type == rttr::type::get<rttr::type>()) {
      bool ok;
      writer.String(var.convert<std::string>(&ok).c_str());
      return true;
    }
    else if (type == rttr::type::get<Anim::Keyframe::ValueType>()) {
      Anim::Keyframe::ValueType const& rawVal{ var.get_value<Anim::Keyframe::ValueType>() };
      if (std::holds_alternative<glm::vec3>(rawVal)) {
        SerializeClassTypes(std::get<glm::vec3>(rawVal), writer);
      }
      return true;
    }

    return false;
  }

  template <typename WriterType>
  void SerializeNextNodes(Anim::Node const& node, WriterType& writer) {
    writer.StartObject();

    writer.Key(JSON_ANIM_NODE_ID_KEY);
    writer.Uint64((uintptr_t)node.get());

    writer.Key("startValue");
    SerializeRecursive(node->startValue, writer);
    writer.Key("endValue");
    SerializeRecursive(node->endValue, writer);
    writer.Key("type");
    SerializeRecursive(node->type, writer);

    writer.Key("startTime");
    writer.Double(node->startTime);
    writer.Key("duration");
    writer.Double(node->duration);

    writer.Key("nextNodes");
    writer.StartArray();
    for (Anim::Node const& next : node->nextNodes) {
      writer.Uint64((uintptr_t)next.get());
    }
    writer.EndArray();

    writer.EndObject();

    // recursively call this on the next node
    for (Anim::Node const& next : node->nextNodes) {
      SerializeNextNodes(next, writer);
    }
  }

#pragma region ScriptsSpecific
  template <typename WriterType>
  void SerializeScriptClassTypes(rttr::instance const& obj, WriterType& writer)
  {
    writer.StartObject();

    rttr::instance wrappedObj{ obj.get_type().is_wrapper() ? obj.get_wrapped_instance() : obj };

    auto const properties{ wrappedObj.get_type().get_properties() };
    for (auto const& property : properties)
    {
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

      if (propVal.is_type<MonoObjectVector>()) {
        propVal = IGE_SCRIPTMGR.SerialMonoObjectVec(propVal.get_value<MonoObjectVector>());
      }

      std::string const name{ property.get_name().to_string() };
      writer.String(name.c_str(), static_cast<rapidjson::SizeType>(name.length()), false);
      if (!SerializeScriptRecursive<WriterType>(propVal, writer))
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

  template <typename WriterType>
  bool SerializeScriptRecursive(rttr::variant const& var, WriterType& writer)
  {
    bool const isWrapper{ var.get_type().is_wrapper() };
    rttr::type const type{ isWrapper ? var.get_type().get_wrapped_type().get_raw_type() :
          var.get_type().is_pointer() ? var.get_type().get_raw_type() : var.get_type() };

    if (WriteBasicTypes<WriterType>(type, isWrapper ? var.extract_wrapped_value() : var, writer)) {

    }
    else if (var.is_sequential_container())
    {
      WriteScriptSequentialContainer<WriterType>(var.create_sequential_view(), writer);
    }
    else if (var.is_associative_container())
    {
      WriteAssociativeContainer<WriterType>(var.create_associative_view(), writer);
    }
    else
    {
      auto properties{ type.get_properties() };
      if (!properties.empty())
      {
        // we only need entity ID and script name for ScriptInstances
        if (type == rttr::type::get<Mono::ScriptInstance>()) {
          Mono::ScriptInstance const& scriptInst{ var.get_value<Mono::ScriptInstance>() };
          writer.StartObject();

          writer.Key(JSON_SCRIPT_ENTITY_ID_KEY);
          writer.Uint(static_cast<uint32_t>(scriptInst.mEntityID));

          writer.Key(JSON_SCRIPT_NAME_KEY);
          writer.String(scriptInst.mScriptName.c_str());

          writer.EndObject();
          return true;
        }
        else {
          SerializeScriptClassTypes<WriterType>(var, writer);
        }
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

  template <typename WriterType>
  void WriteScriptSequentialContainer(rttr::variant_sequential_view const& seqView, WriterType& writer)
  {
    writer.StartArray();

    for (auto const& elem : seqView)
    {
      // if elem is another sequential container, call this function again
      if (elem.is_sequential_container()) {
        WriteScriptSequentialContainer<WriterType>(elem.create_sequential_view(), writer);
      }
      else
      {
        rttr::variant wrappedVar{ elem.extract_wrapped_value() };
        rttr::type const valType{ wrappedVar.get_type() };
        if (!WriteBasicTypes<WriterType>(valType, wrappedVar, writer))
        {
          SerializeScriptClassTypes<WriterType>(wrappedVar, writer);
        }
      }
    }

    writer.EndArray();
  }
#pragma endregion // ScriptsSpecific

  EntityList GetSortedEntities() {
    auto const& entityList{ ECS::EntityManager::GetInstance().GetAllEntities() };
    EntityList entityPQ{};

    for (ECS::Entity e : entityList) { entityPQ.emplace(e); }

    return entityPQ;
  }
}
