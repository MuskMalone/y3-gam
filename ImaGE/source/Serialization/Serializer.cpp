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
#include <pch.h>
#include "Serializer.h"
#include <fstream>

namespace Helper
{
  template <typename T>
  bool IsType(rttr::type const& t) { return t == rttr::type::get<T>(); }
}

namespace Serialization
{

  void Serializer::SerializeAny(rttr::instance const& obj, std::string const& filename)
  {
    std::ofstream ofs{ filename };
    if (!ofs)
    {
#ifdef _DEBUG
      std::cout << "Unable to open file " << filename << "\n";
#endif
      return;
    }

    rapidjson::OStreamWrapper osw{ ofs };
    WriterType writer{ osw };
    SerializeRecursive(obj, writer);
    ofs.close();
  }

  void Serializer::SerializeRecursive(rttr::instance const& obj, WriterType& writer)
  {
    writer.StartObject();

    rttr::instance wrappedObj{ obj.get_type().get_raw_type().is_wrapper() ? obj.get_wrapped_instance() : obj };
    auto const properties{ wrappedObj.get_derived_type().get_properties() };
    for (auto const& property : properties)
    {
      if (property.get_metadata("NO_SERIALIZE")) { continue; }

      rttr::variant propVal{ property.get_value(wrappedObj) };
      if (!propVal)
      {
#ifdef _DEBUG
        std::cout << "Unable to serialize property " << property.get_name().to_string() << " of type " << property.get_type().get_name().to_string() << "\n";
#endif
        continue;
      }

      std::string const name{ property.get_name().to_string() };
      writer.String(name.c_str(), static_cast<rapidjson::SizeType>(name.length()), false);
      if (!WriteVariant(propVal, writer))
      {
#ifdef _DEBUG
        std::cout << "Unable to serialize property " << name << " of type " << property.get_type().get_name().to_string() << "\n";
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
      else if (Helper::IsType<bool>(type)) { writer.Double(var.to_double()); }
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
      if (result)
      {
        writer.String(str.c_str());
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
      if (elem.is_sequential_container())
      {
        WriteSequentialContainer(elem.create_sequential_view(), writer);
      }
      else
      {
        rttr::variant wrappedVar{ elem.extract_wrapped_value() };
        rttr::type const valType{ wrappedVar.get_type() };
        if (!WriteBasicTypes(valType, wrappedVar, writer))
        {
          SerializeRecursive(wrappedVar, writer);
        }
      }
    }

    writer.EndArray();
  }

  bool Serializer::WriteVariant(rttr::variant const& var, WriterType& writer)
  {
    bool const isWrapper{ var.get_type().is_wrapper() };
    rttr::type const type{ isWrapper ? var.get_type().get_wrapped_type().get_raw_type() :
          var.get_type().is_pointer() ? var.get_type().get_raw_type() : var.get_type() };

    if (WriteBasicTypes(type, isWrapper ? var.extract_wrapped_value() : var, writer))
    {

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
        SerializeRecursive(var, writer);
      }
      else
      {
#ifdef _DEBUG
        std::cout << "Unable to write variant of type " << (isWrapper ? type.get_name().to_string() : type.get_name().to_string()) << "\n";
#endif

        return false;
      }
    }

    return true;
  }

  void Serializer::WriteAssociativeContainer(rttr::variant_associative_view const& view, WriterType& writer)
  {
    writer.StartArray();

    if (view.is_key_only_type())
    {
      for (auto const& elem : view)
      {
        WriteVariant(elem.first, writer);
      }
    }
    else
    {
      for (auto const& elem : view)
      {
        writer.StartObject();
        writer.String("key", 3, false);
        WriteVariant(elem.first, writer);

        writer.String("value", 5, false);
        WriteVariant(elem.second, writer);

        writer.EndObject();
      }
    }

    writer.EndArray();
  }

} // namespace Serialization
