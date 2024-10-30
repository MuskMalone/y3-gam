/*!*********************************************************************
\file   ScriptInstance.cpp
\author han.q\@digipen.edu
\date   28-September-2024
\brief
  Script Class to contain the data of a c# class.
  It will store the pointer to the MonoObject and its 4 main methods

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "ScriptInstance.h"
#include "ScriptManager.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/tabledefs.h"
#include "mono/metadata/mono-debug.h"
#include "mono/metadata/threads.h"
#include "Reflection/ProxyScript.h"


using namespace Mono;



ScriptInstance::ScriptInstance(const std::string& scriptName, std::vector<void*>& arg, bool isSpecial) : mCtorType{ (isSpecial) ? SPECIAL_CTOR : (arg.size() == 0) ? DEFAULT_CTOR : ENTITY_CTOR }, mEntityID{(isSpecial) ? entt::null : (arg.size() == 0) ? entt::null : *(static_cast<ECS::Entity::EntityID*>(arg[0]))}, mScriptName{scriptName}
{
  Mono::ScriptManager* sm = &Mono::ScriptManager::GetInstance();
  mScriptClass = sm->GetScriptClass(scriptName);
  if (!mScriptClass)
    throw Debug::Exception<ScriptInstance>(Debug::LVL_WARN, Msg(scriptName + ".cs not found"));
  mClassInst = sm->InstantiateClass(scriptName.c_str(), arg);
  mUpdateMethod = mono_class_get_method_from_name(mScriptClass, "Update", 0);
  //mOnCreateMethod = mono_class_get_method_from_name(mScriptClass, "Create", 0);
  mGcHandle = mono_gchandle_new(mClassInst, true);
  GetAllFieldsInst();
}



void ScriptInstance::FreeScript()
{
  //if (mOnCreateMethod)
  //  monoFree_method(mOnCreateMethod.get());
  //if ( mUpdateMethod)
  //  monoFree_method( mUpdateMethod.get());
  mClassInst = nullptr;
  mScriptClass = nullptr;
  mUpdateMethod = nullptr;

  mScriptFieldInstList.clear();
  mono_gchandle_free(mGcHandle);
}

void ScriptInstance::ReloadScript()
{
  //Clear all the old values
  FreeScript();

  // Load in the new values
  Mono::ScriptManager* sm = &Mono::ScriptManager::GetInstance();
  mScriptClass = sm->GetScriptClass(mScriptName);
  std::vector<void*> arg{};
  if (mCtorType == ENTITY_CTOR)
    arg.push_back(&mEntityID);
  mClassInst = sm->InstantiateClass(mScriptName.c_str(), arg);
  mUpdateMethod = mono_class_get_method_from_name(mScriptClass, "OnUpdate", 1);
  mGcHandle = mono_gchandle_new(mClassInst, true);
  GetAllFieldsInst();
  SetAllFields();
}

void ScriptInstance::InvokeOnUpdate(double dt)
{
  if ( mUpdateMethod)
  {
    std::vector<void*> params = { &dt };
    mono_runtime_invoke( mUpdateMethod, mono_gchandle_get_target(mGcHandle), params.data(), nullptr);
  }
}


void ScriptInstance::GetFieldCSClass(std::vector<rttr::variant>& mScriptFieldInstList, const Mono::ScriptFieldInfo& field)
{
  DataMemberInstance<ScriptInstance> sfi{ field };
  if (!sfi.mData.mClassInst)
  {
    Mono::ScriptManager* sm = &Mono::ScriptManager::GetInstance();
    sfi.mData.mClassInst = mono_field_get_value_object(sm->mAppDomain.get(), sfi.mScriptField.mClassField, mClassInst);

    if (sfi.mData.mClassInst)
    {
      sfi.mData.mScriptClass = mono_object_get_class(sfi.mData.mClassInst);
      sfi.mData.mScriptName = mono_class_get_name(sfi.mData.mScriptClass);
      sfi.mData.GetAllFieldsInst();
    }
  }

  mScriptFieldInstList.emplace_back(sfi);
}

void ScriptInstance::GetAllFieldsInst()
{
  mScriptFieldInstList.clear();
  Mono::ScriptManager* sm = &Mono::ScriptManager::GetInstance();
  ScriptClassInfo sci = sm->GetScriptClassInfo(mScriptName);
  const auto& fields = sci.mScriptFieldMap;
  for (const auto& [fieldName, field] : fields)
  {

    switch (field.mFieldType)
    {
      case (ScriptFieldType::BOOL):
      {
        bool value = GetFieldValue<bool>(field.mClassField);
        DataMemberInstance<bool> test{ field,value };
        mScriptFieldInstList.emplace_back(test);
        break;
      }
      case (ScriptFieldType::FLOAT):
      {
        float value = GetFieldValue<float>(field.mClassField);
        DataMemberInstance<float> test{ field,value };
        mScriptFieldInstList.emplace_back(test);
        break;
      }
       case (ScriptFieldType::INT):
      {
        int value = GetFieldValue<int>(field.mClassField);
        DataMemberInstance<int> test{ field,value };
        mScriptFieldInstList.emplace_back(test);
        break;
      }
       case (ScriptFieldType::DOUBLE):
      {
        double value = GetFieldValue<double>(field.mClassField);
        DataMemberInstance<double> test{ field,value };
        mScriptFieldInstList.emplace_back(test);
        break;
      }
      case (ScriptFieldType::STRING):
      {
        MonoString* value = GetFieldValue<MonoString*>(field.mClassField);
        std::string const str{ MonoStringToSTD(value) };
        DataMemberInstance<std::string> sfi{ field, str };
        mScriptFieldInstList.emplace_back(sfi);
        break;
      }
       case (ScriptFieldType::DVEC3):
      {
        glm::dvec3 value = GetFieldValue<glm::dvec3>(field.mClassField);
        DataMemberInstance<glm::dvec3> test{ field,value };
        mScriptFieldInstList.emplace_back(test);
        break;
      }

       case (ScriptFieldType::VEC3):
       {
         glm::vec3 value = GetFieldValue<glm::vec3>(field.mClassField);
         DataMemberInstance<glm::vec3> test{ field,value };
         mScriptFieldInstList.emplace_back(test);
         break;
       }
       case (ScriptFieldType::INT_ARR):
      {
        std::vector<int> value = GetFieldValueArr<int>(field.mClassField);
        DataMemberInstance<std::vector<int>> test{ field,value };
        mScriptFieldInstList.emplace_back(test);
        break;
      }
       case (ScriptFieldType::UINT_ARR):
      {

        std::vector<unsigned> value = GetFieldValueArr<unsigned>(field.mClassField);
        DataMemberInstance<std::vector<unsigned>> test{ field,value };
        mScriptFieldInstList.emplace_back(test);
        break;
      }
       case (ScriptFieldType::STRING_ARR):
      {
        std::vector<MonoString*> value = GetFieldValueArr<MonoString*>(field.mClassField);
        std::vector<std::string> proxy{};
        for (MonoString* s : value)
        {
          proxy.push_back(MonoStringToSTD(s));
        }
        DataMemberInstance<std::vector<std::string>> test{ field, proxy };
        mScriptFieldInstList.emplace_back(test);
        break;
      }
      case (ScriptFieldType::ENTITY):
      {
        GetFieldCSClass(mScriptFieldInstList, field);
        break;
      }
    }
  }
}

void ScriptInstance::SetAllFields()
{
  Mono::ScriptManager* sm = &Mono::ScriptManager::GetInstance();
  ScriptClassInfo sci = sm->GetScriptClassInfo(mScriptName);

  for (rttr::variant& f : mScriptFieldInstList)
  {
    if (f.is_type<Mono::DataMemberInstance<float>>())
    {
      Mono::DataMemberInstance<float>& sfi = f.get_value<Mono::DataMemberInstance<float>>();
      SetFieldValue<float>(sfi.mData, sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<int>>())
    {
      Mono::DataMemberInstance<int>& sfi = f.get_value<Mono::DataMemberInstance<int>>();
      SetFieldValue<int>(sfi.mData, sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<double>>())
    {
      Mono::DataMemberInstance<double>& sfi = f.get_value<Mono::DataMemberInstance<double>>();
      SetFieldValue<double>(sfi.mData, sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<std::string>>())
    {
      Mono::DataMemberInstance<std::string>& sfi = f.get_value<Mono::DataMemberInstance<std::string>>();
      mono_field_set_value(mClassInst, sfi.mScriptField.mClassField, STDToMonoString(sfi.mData));
    }

    else if (f.is_type<Mono::DataMemberInstance<glm::dvec3>>())
    {
      Mono::DataMemberInstance<glm::dvec3>& sfi = f.get_value<Mono::DataMemberInstance<glm::dvec3>>();
      SetFieldValue<glm::dvec3>(sfi.mData, sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<std::vector<int>>>())
    {
      Mono::DataMemberInstance<std::vector<int>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<int>>>();
      SetFieldValueArr<int>(sfi.mData,sfi.mScriptField.mClassField,sm->mAppDomain);
    }
    else if (f.is_type<Mono::DataMemberInstance<std::vector<unsigned>>>())
    {
      Mono::DataMemberInstance<std::vector<unsigned>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<unsigned>>>();
      SetFieldValueArr<unsigned>(sfi.mData, sfi.mScriptField.mClassField, sm->mAppDomain);
    }
    else if (f.is_type<Mono::DataMemberInstance<std::vector<std::string>>>())
    {
      Mono::DataMemberInstance<std::vector<std::string>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<std::string>>>();
      std::vector<MonoString*> proxy{};
      for (std::string s : sfi.mData)
      {
        proxy.push_back(STDToMonoString(s));
      }
      SetFieldValueArr<MonoString*>(proxy,sfi.mScriptField.mClassField, sm->mAppDomain);
    }
    else if (f.is_type<Mono::DataMemberInstance<ScriptInstance>>())
    {
      Mono::DataMemberInstance<ScriptInstance>& sfi = f.get_value<Mono::DataMemberInstance<ScriptInstance>>();
      if (!sfi.mData.mClassInst)
        sfi.mData.SetAllFields();

    }
  }

}

void ScriptInstance::SetAllFields(std::vector<rttr::variant> scriptFieldProxyList)
{
  Mono::ScriptManager* sm = &Mono::ScriptManager::GetInstance();
  //ScriptClassInfo sci = sm->GetScriptClassInfo(mScriptName);
  for (const rttr::variant& i : scriptFieldProxyList)
  {
    for (rttr::variant& f : mScriptFieldInstList)
    {
      if (f.is_type<Mono::DataMemberInstance<float>>() && i.is_type<Reflection::ProxySFInfo<float>>())
      {
        Mono::DataMemberInstance<float>& sfi = f.get_value<Mono::DataMemberInstance<float>>();
        const Reflection::ProxySFInfo<float>& psi = i.get_value<Reflection::ProxySFInfo<float>>();
        if (sfi.mScriptField.mFieldName == psi.fieldName)
        {
          sfi.mData = psi.data;
          SetFieldValue<float>(sfi.mData, sfi.mScriptField.mClassField);
        }
      }
      else if (f.is_type<Mono::DataMemberInstance<int>>() && i.is_type<Reflection::ProxySFInfo<int>>())
      {
        Mono::DataMemberInstance<int>& sfi = f.get_value<Mono::DataMemberInstance<int>>();
        const Reflection::ProxySFInfo<int>& psi = i.get_value<Reflection::ProxySFInfo<int>>();
        if (sfi.mScriptField.mFieldName == psi.fieldName)
        {
          sfi.mData = psi.data;
          SetFieldValue<int>(sfi.mData, sfi.mScriptField.mClassField);
        }
      }
      else if (f.is_type<Mono::DataMemberInstance<double>>() && i.is_type<Reflection::ProxySFInfo<double>>())
      {
        Mono::DataMemberInstance<double>& sfi = f.get_value<Mono::DataMemberInstance<double>>();
        const Reflection::ProxySFInfo<double>& psi = i.get_value<Reflection::ProxySFInfo<double>>();
        if (sfi.mScriptField.mFieldName == psi.fieldName)
        {
          sfi.mData = psi.data;
          SetFieldValue<double>(sfi.mData, sfi.mScriptField.mClassField);
        }
      }
      else if (f.is_type<Mono::DataMemberInstance<std::string>>() && i.is_type<Reflection::ProxySFInfo<std::string>>())
      {
        Mono::DataMemberInstance<std::string>& sfi = f.get_value<Mono::DataMemberInstance<std::string>>();
        const Reflection::ProxySFInfo<std::string>& psi = i.get_value<Reflection::ProxySFInfo<std::string>>();
        if (sfi.mScriptField.mFieldName == psi.fieldName)
        {
          sfi.mData = psi.data;
          mono_field_set_value(mClassInst, sfi.mScriptField.mClassField, STDToMonoString(sfi.mData));
        }
      }
      else if (f.is_type<Mono::DataMemberInstance<glm::dvec3>>() && i.is_type<Reflection::ProxySFInfo<glm::dvec3>>())
      {
        Mono::DataMemberInstance<glm::dvec3>& sfi = f.get_value<Mono::DataMemberInstance<glm::dvec3>>();
        const Reflection::ProxySFInfo<glm::dvec3>& psi = i.get_value<Reflection::ProxySFInfo<glm::dvec3>>();
        if (sfi.mScriptField.mFieldName == psi.fieldName)
        {
          sfi.mData = psi.data;
          SetFieldValue<glm::dvec3>(sfi.mData, sfi.mScriptField.mClassField);
        }
      }
      else if (f.is_type<Mono::DataMemberInstance<std::vector<int>>>() && i.is_type<Reflection::ProxySFInfo<std::vector<int>>>())
      {
        Mono::DataMemberInstance<std::vector<int>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<int>>>();
        const Reflection::ProxySFInfo<std::vector<int>>& psi = i.get_value<Reflection::ProxySFInfo<std::vector<int>>>();
        if (sfi.mScriptField.mFieldName == psi.fieldName)
        {
          sfi.mData = psi.data;
          SetFieldValueArr<int>(sfi.mData, sfi.mScriptField.mClassField, sm->mAppDomain);
        }
      }
      else if (f.is_type<Mono::DataMemberInstance<std::vector<unsigned>>>() && i.is_type<Reflection::ProxySFInfo<std::vector<unsigned>>>())
      {
        Mono::DataMemberInstance<std::vector<unsigned>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<unsigned>>>();
        const Reflection::ProxySFInfo<std::vector<unsigned>>& psi = i.get_value<Reflection::ProxySFInfo<std::vector<unsigned>>>();
        if (sfi.mScriptField.mFieldName == psi.fieldName)
        {
          sfi.mData = psi.data;
          SetFieldValueArr<unsigned>(sfi.mData, sfi.mScriptField.mClassField, sm->mAppDomain);
        }
      }
      else if (f.is_type<Mono::DataMemberInstance<std::vector<std::string>>>() && i.is_type<Reflection::ProxySFInfo<std::vector<std::string>>>())
      {
        Mono::DataMemberInstance<std::vector<std::string>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<std::string>>>();
        const Reflection::ProxySFInfo<std::vector<std::string>>& psi = i.get_value<Reflection::ProxySFInfo<std::vector<std::string>>>();
        if (sfi.mScriptField.mFieldName == psi.fieldName)
        {
          sfi.mData = psi.data;
          std::vector<MonoString*> proxy{};
          for (const std::string& s : sfi.mData)
          {
            proxy.push_back(STDToMonoString(s));
          }
          SetFieldValueArr<MonoString*>(proxy, sfi.mScriptField.mClassField, sm->mAppDomain);
        }
      }
      else if (f.is_type<Mono::DataMemberInstance<ScriptInstance>>() && i.is_type<Reflection::ProxySFInfo<ScriptInstance>>())
      {
        Mono::DataMemberInstance<ScriptInstance>& sfi = f.get_value<Mono::DataMemberInstance<ScriptInstance>>();
        const Reflection::ProxySFInfo<ScriptInstance>& psi = i.get_value<Reflection::ProxySFInfo<ScriptInstance>>();
        if (psi.data.mClassInst)
        {
          if (sfi.mScriptField.mFieldName == psi.fieldName)
          {
            sfi.mData = psi.data;
            SetFieldValue<MonoObject*>(sfi.mData.mClassInst, sfi.mScriptField.mClassField);
          }
        }
      }

    }
  }

  

}


void ScriptInstance::GetAllUpdatedFields()
{
  Mono::ScriptManager* sm = &Mono::ScriptManager::GetInstance();
  ScriptClassInfo sci = sm->GetScriptClassInfo(mScriptName);

  for (rttr::variant& f : mScriptFieldInstList)
  {
    if (f.is_type<Mono::DataMemberInstance<float>>())
    {
      Mono::DataMemberInstance<float>& sfi = f.get_value<Mono::DataMemberInstance<float>>();
      sfi.mData = GetFieldValue<float>(sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<int>>())
    {
      Mono::DataMemberInstance<int>& sfi = f.get_value<Mono::DataMemberInstance<int>>();
      sfi.mData = GetFieldValue<int>(sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<double>>())
    {
      Mono::DataMemberInstance<double>& sfi = f.get_value<Mono::DataMemberInstance<double>>();
      sfi.mData = GetFieldValue<double>(sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<std::string>>())
    {
      Mono::DataMemberInstance<std::string>& sfi = f.get_value<Mono::DataMemberInstance<std::string>>();
      sfi.mData = MonoStringToSTD(GetFieldValue<MonoString*>(sfi.mScriptField.mClassField));
    }
    else if (f.is_type<Mono::DataMemberInstance<glm::dvec3>>())
    {
      Mono::DataMemberInstance<glm::dvec3>& sfi = f.get_value<Mono::DataMemberInstance<glm::dvec3>>();
      sfi.mData = GetFieldValue<glm::dvec3>(sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<std::vector<int>>>())
    {
      Mono::DataMemberInstance<std::vector<int>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<int>>>();
      sfi.mData = GetFieldValueArr<int>(sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<std::vector<unsigned>>>())
    {
      Mono::DataMemberInstance<std::vector<unsigned>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<unsigned>>>();
      sfi.mData = GetFieldValueArr<unsigned>(sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<std::vector<std::string>>>())
    {
      Mono::DataMemberInstance<std::vector<std::string>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<std::string>>>();
      std::vector<MonoString*> proxy = GetFieldValueArr<MonoString*>(sfi.mScriptField.mClassField);
      sfi.mData.clear();
      for (MonoString* s : proxy)
      {
        sfi.mData.push_back(MonoStringToSTD(s));
      }
    }
    else if (f.is_type<Mono::DataMemberInstance<ScriptInstance>>())
    {
      Mono::DataMemberInstance<ScriptInstance>& sfi = f.get_value<Mono::DataMemberInstance<ScriptInstance>>();
      if (!sfi.mData.mClassInst)
        sfi.mData.GetAllUpdatedFields();
    }
  }
}



void ScriptInstance::PrintAllField()
{
  Mono::ScriptManager* sm = &Mono::ScriptManager::GetInstance();
  ScriptClassInfo sci = sm->GetScriptClassInfo(mScriptName);

  for (rttr::variant& f : mScriptFieldInstList)
  {
    rttr::type dataType{ f.get_type() };
    if ((dataType == rttr::type::get<Mono::DataMemberInstance<float>>()))
    {
      Mono::DataMemberInstance<float>& sfi = f.get_value<Mono::DataMemberInstance<float>>();
      std::cout << sfi.mScriptField.mFieldName << ": " << GetFieldValue<float>(sfi.mScriptField.mClassField) << "\n";
    }
    else if ((dataType == rttr::type::get<Mono::DataMemberInstance<int>>()))
    {
      Mono::DataMemberInstance<int>& sfi = f.get_value<Mono::DataMemberInstance<int>>();
      std::cout << sfi.mScriptField.mFieldName << ": " << GetFieldValue<int>(sfi.mScriptField.mClassField) << "\n";
    }
    else if ((dataType == rttr::type::get<Mono::DataMemberInstance<double>>()))
    {
      Mono::DataMemberInstance<double>& sfi = f.get_value<Mono::DataMemberInstance<double>>();
      std::cout << sfi.mScriptField.mFieldName << ": " << GetFieldValue<double>(sfi.mScriptField.mClassField) << "\n";
    }
    else if ((dataType == rttr::type::get<Mono::DataMemberInstance<glm::dvec3>>()))
    {
      Mono::DataMemberInstance<glm::dvec3>& sfi = f.get_value<Mono::DataMemberInstance<glm::dvec3>>();
      glm::dvec3 data = GetFieldValue<glm::dvec3>(sfi.mScriptField.mClassField);
      std::cout << sfi.mScriptField.mFieldName << ": " << data.x << "," << data.y << "," << data.z << "\n";
    }

    else if ((dataType == rttr::type::get<Mono::DataMemberInstance<std::vector<int>>>()))
    {
      Mono::DataMemberInstance<std::vector<int>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<int>>>();
      std::vector<int> data = GetFieldValueArr<int>(sfi.mScriptField.mClassField);
      std::cout << sfi.mScriptField.mFieldName << ": ";
      for (int i : data)
        std::cout << i << ", ";
      std::cout << "\n";
    }

    else if ((dataType == rttr::type::get<Mono::DataMemberInstance<std::vector<unsigned>>>()))
    {
      Mono::DataMemberInstance<std::vector<unsigned>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<unsigned>>>();
      std::vector<unsigned> data = GetFieldValueArr<unsigned>(sfi.mScriptField.mClassField);
      std::cout << sfi.mScriptField.mFieldName << ": ";
      for (unsigned i : data)
        std::cout << i << ", ";
      std::cout << "\n";
    }
  }
}



void ScriptInstance::SetEntityID(ECS::Entity::EntityID entityId)
{
  if (!mScriptClass)
    throw Debug::Exception<ScriptInstance>(Debug::LVL_WARN, Msg(mScriptName + ".cs not found"));
  mEntityID = entityId;
  MonoClass* parent = mono_class_get_parent(mScriptClass);
  if (!parent || std::string(mono_class_get_name(parent)) != "Entity") return;
  MonoMethod* setEntityIDMethod = mono_class_get_method_from_name(parent, "SetEntityID", 1);
  std::vector<void*> params = { &entityId };
  mono_runtime_invoke(setEntityIDMethod, mono_gchandle_get_target(mGcHandle), params.data(), nullptr);
}