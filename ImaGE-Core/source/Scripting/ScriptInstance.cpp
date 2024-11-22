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
#include "Input/InputManager.h"
#include <algorithm>

using namespace Mono;



ScriptInstance::ScriptInstance(const std::string& scriptName) : mScriptName{scriptName}  //Ctor for c# scripts that follow monobehaviour
{
  Mono::ScriptManager* sm = &Mono::ScriptManager::GetInstance();
  mScriptClass = sm->GetScriptClass(scriptName);
  if (!mScriptClass)
    throw Debug::Exception<ScriptInstance>(Debug::LVL_WARN, Msg(scriptName + ".cs not found"));
    mClassInst = sm->InstantiateClass(scriptName.c_str());   // All C# script with Monobehaviour will be default constructed
  
  mUpdateMethod = mono_class_get_method_from_name(mScriptClass, "Update", 0);

  //mOnCreateMethod = mono_class_get_method_from_name(mScriptClass, "Create", 0);
  mGcHandle = mono_gchandle_new(mClassInst, true);
  GetAllFieldsInst();
  ScriptManager::GetInstance().mTriggerStart = true;
}

ScriptInstance::ScriptInstance(MonoObject* mo, bool setEntityID, bool forSerialization)  //ctor to create temp scriptinstance (only for displaying std::vector<MonoObject*> in inspector)
{
  Mono::ScriptManager* sm = &Mono::ScriptManager::GetInstance();
  mScriptClass = mono_object_get_class(mo);
  mScriptName = mono_class_get_name(mScriptClass);
  if (!mScriptClass)
    throw Debug::Exception<ScriptInstance>(Debug::LVL_WARN, Msg("Trying to create a script instance using an invalid MonoObject"));
  mClassInst = mo;   // All C# script with Monobehaviour will be default constructed
  GetAllFieldsInst();
  if (setEntityID)
  {
    mGcHandle = mono_gchandle_new(mClassInst, true);
    SetEntityID(static_cast<ECS::Entity::EntityID>(std::numeric_limits<unsigned>().max()));
    mono_gchandle_free(mGcHandle);
  }
  if(forSerialization)
    mEntityID = static_cast<ECS::Entity::EntityID>(mScriptFieldInstList[0].get_value<Mono::DataMemberInstance<unsigned>>().mData);
  
    
}

void ScriptInstance::ReplaceEntity(MonoObject* mo)
{
  mClassInst = mo;
  if (mGcHandle)
    mono_gchandle_free(mGcHandle);
  mGcHandle = mono_gchandle_new(mClassInst, true);
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
  if(mGcHandle)
    mono_gchandle_free(mGcHandle);
}

void ScriptInstance::SetEntityID(ECS::Entity::EntityID entityID)
{
  mEntityID = entityID;
  std::vector<void*> arg{ &mEntityID };
  MonoMethod* InitMethod = mono_class_get_method_from_name(mono_class_get_parent(mScriptClass), "Init", 1);
  if (InitMethod)
  {
    mono_runtime_invoke(InitMethod, mono_gchandle_get_target(mGcHandle), arg.data(), nullptr);  // We will call an init function to pass in the entityID
  }
  else
  {
    MonoMethod* InitMethod = mono_class_get_method_from_name(mScriptClass, "Init", 1);
    if (InitMethod)
    {
      if(mGcHandle)
       mono_runtime_invoke(InitMethod, mono_gchandle_get_target(mGcHandle), arg.data(), nullptr);  // We will call an init function to pass in the entityID
      else
        mono_runtime_invoke(InitMethod, mClassInst, arg.data(), nullptr);  // We will call an init function to pass in the entityID
    }
    else
      Debug::DebugLogger::GetInstance().LogError("you are trying to pass an entityID to a script that doesn't inherit from entity");
  }
}


void ScriptInstance::ReloadScript()
{
  //Clear all the old values
  FreeScript();

  // Load in the new values
  Mono::ScriptManager* sm = &Mono::ScriptManager::GetInstance();
  mScriptClass = sm->GetScriptClass(mScriptName);
  std::vector<void*> arg{};
  mClassInst = sm->InstantiateClass(mScriptName.c_str());
  SetEntityID(mEntityID);
  mUpdateMethod = mono_class_get_method_from_name(mScriptClass, "OnUpdate", 0);
  mGcHandle = mono_gchandle_new(mClassInst, true);
  GetAllFieldsInst();
  SetAllFields();
}

void ScriptInstance::InvokeOnUpdate()
{
  glm::vec2 delt = Input::InputManager::GetInstance().GetMouseDelta();
  if ( mUpdateMethod)
  {
    std::vector<void*> params = { };
    mono_runtime_invoke( mUpdateMethod, mono_gchandle_get_target(mGcHandle), params.data(), nullptr);
  }
}

void ScriptInstance::InvokeStart()
{
  if (!mHasStarted) // We have not call the start() func, so we wil trigger it once
  {
    mHasStarted = true;
    MonoMethod* startMethod = mono_class_get_method_from_name(mScriptClass, "Start", 0);
    if (startMethod)
    {
      std::vector<void*> params = { };
      mono_runtime_invoke(startMethod, mono_gchandle_get_target(mGcHandle), params.data(), nullptr);
    }
  }
}


void ScriptInstance::GetFieldCSClass(std::vector<rttr::variant>& mScriptFieldInstList, const Mono::ScriptFieldInfo& field)
{
  DataMemberInstance<ScriptInstance> sfi{ field };
  if (!sfi.mData.mClassInst)
  {
    Mono::ScriptManager* sm = &Mono::ScriptManager::GetInstance();
    sfi.mData.mClassInst = mono_field_get_value_object(sm->mAppDomain.get(), sfi.mScriptField.mClassField, mClassInst);
    sfi.mData.mScriptName = sm->mRevClassMap[sfi.mScriptField.mFieldType];
    if (sfi.mData.mClassInst)
    {
      sfi.mData.mScriptClass = mono_object_get_class(sfi.mData.mClassInst);
      //sfi.mData.mScriptName = mono_class_get_name(sfi.mData.mScriptClass);
     //* sfi.mData.mScriptName = sfi.mData.mScriptName + '.' + mono_class_get_name(sfi.mData.mScriptClass);
      sfi.mData.GetAllFieldsInst();
    }
    else
      sfi.mData.mEntityID = static_cast<ECS::Entity::EntityID>(-1);
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
      case (ScriptFieldType::UINT):
      {
        unsigned value = GetFieldValue<unsigned>(field.mClassField);
        DataMemberInstance<unsigned> test{ field,value };
        mScriptFieldInstList.emplace_back(test);
        break;
      }
      case (ScriptFieldType::BOOL):
      {
        bool value = GetFieldValue<bool>(field.mClassField);
        DataMemberInstance<bool> test{ field,value };
        mScriptFieldInstList.emplace_back(test);
        break;
      }
      case (ScriptFieldType::SHORT):
      {
        short value = GetFieldValue<short>(field.mClassField);
        DataMemberInstance<short> test{ field,value };
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
      case (ScriptFieldType::FLOAT):
      {
        float value = GetFieldValue<float>(field.mClassField);
        DataMemberInstance<float> test{ field,value };
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
       case (ScriptFieldType::LONG):
       {
         int64_t value = GetFieldValue<int64_t>(field.mClassField);
         DataMemberInstance<int64_t> test{ field,value };
         mScriptFieldInstList.emplace_back(test);
         break;
       }
       case (ScriptFieldType::USHORT):
       {
         uint16_t value = GetFieldValue<uint16_t>(field.mClassField);
         DataMemberInstance<uint16_t> test{ field,value };
         mScriptFieldInstList.emplace_back(test);
         break;
       }
       case (ScriptFieldType::ULONG):
       {
         uint64_t value = GetFieldValue<uint64_t>(field.mClassField);
         DataMemberInstance<uint64_t> test{ field,value };
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
      case (ScriptFieldType::VEC3):
      {
        glm::vec3 value = GetFieldValue<glm::vec3>(field.mClassField);
        DataMemberInstance<glm::vec3> test{ field,value };
        mScriptFieldInstList.emplace_back(test);
        break;
      }
      case (ScriptFieldType::DVEC3):
      {
        glm::dvec3 value = GetFieldValue<glm::dvec3>(field.mClassField);
        DataMemberInstance<glm::dvec3> test{ field,value };
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
       case (ScriptFieldType::FLOAT_ARR):
       {
         std::vector<float> value = GetFieldValueArr<float>(field.mClassField);
         DataMemberInstance<std::vector<float>> test{ field,value };
         mScriptFieldInstList.emplace_back(test);
         break;
       }
       case (ScriptFieldType::DOUBLE_ARR):
       {
         std::vector<double> value = GetFieldValueArr<double>(field.mClassField);
         DataMemberInstance<std::vector<double>> test{ field,value };
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
       case (ScriptFieldType::UINT_ARR):
      {
        std::vector<unsigned> value = GetFieldValueArr<unsigned>(field.mClassField);
        DataMemberInstance<std::vector<unsigned>> test{ field,value };
        mScriptFieldInstList.emplace_back(test);
        break;
      }
       case (ScriptFieldType::ENTITY_ARR):
       {
         std::vector<MonoObject*> value = GetFieldValueArr<MonoObject*>(field.mClassField);
         DataMemberInstance<std::vector<MonoObject*>> test{ field,value };
         mScriptFieldInstList.emplace_back(test);
         break;
       }
      case (ScriptFieldType::ENTITY):
      {
        GetFieldCSClass(mScriptFieldInstList, field);
        Mono::DataMemberInstance<ScriptInstance>& sfi = mScriptFieldInstList[mScriptFieldInstList.size() - 1].get_value<Mono::DataMemberInstance<ScriptInstance>>();
        if (sfi.mData.mClassInst)
          sfi.mData.mEntityID = static_cast<ECS::Entity::EntityID>(sfi.mData.mScriptFieldInstList[0].get_value<Mono::DataMemberInstance<unsigned>>().mData);
        break;
      }
   

      default: // The rest of the data types will be public member of type c# scripts that inherits from entity
      {
        GetFieldCSClass(mScriptFieldInstList, field);
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
    if (f.is_type<Mono::DataMemberInstance<unsigned>>())
    {
       Mono::DataMemberInstance<unsigned>& sfi = f.get_value<Mono::DataMemberInstance<unsigned>>();
       SetFieldValue<unsigned>(sfi.mData, sfi.mScriptField.mClassField);
    }
    if (f.is_type<Mono::DataMemberInstance<bool>>())
    {
      Mono::DataMemberInstance<bool>& sfi = f.get_value<Mono::DataMemberInstance<bool>>();
      SetFieldValue<bool>(sfi.mData, sfi.mScriptField.mClassField);
    }
    if (f.is_type<Mono::DataMemberInstance<short>>())
    {
      Mono::DataMemberInstance<short>& sfi = f.get_value<Mono::DataMemberInstance<short>>();
      SetFieldValue<short>(sfi.mData, sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<int>>())
    {
      Mono::DataMemberInstance<int>& sfi = f.get_value<Mono::DataMemberInstance<int>>();
      SetFieldValue<int>(sfi.mData, sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<float>>())
    {
      Mono::DataMemberInstance<float>& sfi = f.get_value<Mono::DataMemberInstance<float>>();
      SetFieldValue<float>(sfi.mData, sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<double>>())
    {
      Mono::DataMemberInstance<double>& sfi = f.get_value<Mono::DataMemberInstance<double>>();
      SetFieldValue<double>(sfi.mData, sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<int64_t>>())
    {
      Mono::DataMemberInstance<int64_t>& sfi = f.get_value<Mono::DataMemberInstance<int64_t>>();
      SetFieldValue<int64_t>(sfi.mData, sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<uint16_t>>())
    {
      Mono::DataMemberInstance<uint16_t>& sfi = f.get_value<Mono::DataMemberInstance<uint16_t>>();
      SetFieldValue<uint16_t>(sfi.mData, sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<uint64_t>>())
    {
      Mono::DataMemberInstance<uint64_t>& sfi = f.get_value<Mono::DataMemberInstance<uint64_t>>();
      SetFieldValue<uint64_t>(sfi.mData, sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<std::string>>())
    {
      Mono::DataMemberInstance<std::string>& sfi = f.get_value<Mono::DataMemberInstance<std::string>>();
      mono_field_set_value(mClassInst, sfi.mScriptField.mClassField, STDToMonoString(sfi.mData));
    }
    else if (f.is_type<Mono::DataMemberInstance<glm::vec3>>())
    {
      Mono::DataMemberInstance<glm::vec3>& sfi = f.get_value<Mono::DataMemberInstance<glm::vec3>>();
      SetFieldValue<glm::vec3>(sfi.mData, sfi.mScriptField.mClassField);
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
    else if (f.is_type<Mono::DataMemberInstance<std::vector<float>>>())
    {
      Mono::DataMemberInstance<std::vector<float>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<float>>>();
      SetFieldValueArr<float>(sfi.mData, sfi.mScriptField.mClassField, sm->mAppDomain);
    }
    else if (f.is_type<Mono::DataMemberInstance<std::vector<double>>>())
    {
      Mono::DataMemberInstance<std::vector<double>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<double>>>();
      SetFieldValueArr<double>(sfi.mData, sfi.mScriptField.mClassField, sm->mAppDomain);
    }
    else if (f.is_type<Mono::DataMemberInstance<std::vector<MonoObject*>>>())
    {
      Mono::DataMemberInstance<std::vector<MonoObject*>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<MonoObject*>>>();
      SetFieldValueArr<MonoObject*>(sfi.mData, sfi.mScriptField.mClassField, sm->mAppDomain);
    }
    else if (f.is_type<Mono::DataMemberInstance<std::vector<std::string>>>())
    {
      Mono::DataMemberInstance<std::vector<std::string>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<std::string>>>();
      std::vector<MonoString*> proxy{};
      for (std::string s : sfi.mData)
      {
        proxy.push_back(STDToMonoString(s));
      }
      SetFieldValueArr<MonoString*>(proxy, sfi.mScriptField.mClassField, sm->mAppDomain);
    }
    else if (f.is_type<Mono::DataMemberInstance<std::vector<unsigned>>>())
    {
      Mono::DataMemberInstance<std::vector<unsigned>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<unsigned>>>();
      SetFieldValueArr<unsigned>(sfi.mData, sfi.mScriptField.mClassField, sm->mAppDomain);
    }
    else if (f.is_type<Mono::DataMemberInstance<ScriptInstance>>())
    {
      Mono::DataMemberInstance<ScriptInstance>& sfi = f.get_value<Mono::DataMemberInstance<ScriptInstance>>();
      if (!sfi.mData.mClassInst)
        sfi.mData.SetAllFields();

    }
  }

}

void ScriptInstance::SetAllFields(std::vector<rttr::variant> const& scriptFieldProxyList)
{
  Mono::ScriptManager* sm = &Mono::ScriptManager::GetInstance();
  //ScriptClassInfo sci = sm->GetScriptClassInfo(mScriptName);
  for (const rttr::variant& i : scriptFieldProxyList)
  {
    for (rttr::variant& f : mScriptFieldInstList)
    {
       if (f.is_type<Mono::DataMemberInstance<unsigned>>() && i.get_type() == f.get_type())
       {
         Mono::DataMemberInstance<unsigned>& sfi = f.get_value<Mono::DataMemberInstance<unsigned>>();
         const Mono::DataMemberInstance<unsigned>& psi = i.get_value<Mono::DataMemberInstance<unsigned>>();
         if (sfi.mScriptField.mFieldName == psi.mScriptField.mFieldName)
         {
           sfi.mData = psi.mData;
           SetFieldValue<unsigned>(sfi.mData, sfi.mScriptField.mClassField);
         }
         break;
       }
       else if (f.is_type<Mono::DataMemberInstance<bool>>() && i.get_type() == f.get_type())
       {
         Mono::DataMemberInstance<bool>& sfi = f.get_value<Mono::DataMemberInstance<bool>>();
         const Mono::DataMemberInstance<bool>& psi = i.get_value<Mono::DataMemberInstance<bool>>();
         if (sfi.mScriptField.mFieldName == psi.mScriptField.mFieldName)
         {
           sfi.mData = psi.mData;
           SetFieldValue<bool>(sfi.mData, sfi.mScriptField.mClassField);
         }
         break;
       }
       else if (f.is_type<Mono::DataMemberInstance<short>>() && i.get_type() == f.get_type())
       {
         Mono::DataMemberInstance<short>& sfi = f.get_value<Mono::DataMemberInstance<short>>();
         const Mono::DataMemberInstance<short>& psi = i.get_value<Mono::DataMemberInstance<short>>();
         if (sfi.mScriptField.mFieldName == psi.mScriptField.mFieldName)
         {
           sfi.mData = psi.mData;
           SetFieldValue<short>(sfi.mData, sfi.mScriptField.mClassField);
         }
         break;
       }
      else if (f.is_type<Mono::DataMemberInstance<int>>() && i.get_type() == f.get_type())
      {
        Mono::DataMemberInstance<int>& sfi = f.get_value<Mono::DataMemberInstance<int>>();
        const Mono::DataMemberInstance<int>& psi = i.get_value<Mono::DataMemberInstance<int>>();
        if (sfi.mScriptField.mFieldName == psi.mScriptField.mFieldName)
        {
          sfi.mData = psi.mData;
          SetFieldValue<int>(sfi.mData, sfi.mScriptField.mClassField);
        }
        break;
      }
      else if (f.is_type<Mono::DataMemberInstance<float>>() && i.get_type() == f.get_type())
       {
         Mono::DataMemberInstance<float>& sfi = f.get_value<Mono::DataMemberInstance<float>>();
         const Mono::DataMemberInstance<float>& psi = i.get_value<Mono::DataMemberInstance<float>>();
         if (sfi.mScriptField.mFieldName == psi.mScriptField.mFieldName)
         {
           sfi.mData = psi.mData;
           SetFieldValue<float>(sfi.mData, sfi.mScriptField.mClassField);
         }
         break;
       }
      else if (f.is_type<Mono::DataMemberInstance<double>>() && i.get_type() == f.get_type())
      {
        Mono::DataMemberInstance<double>& sfi = f.get_value<Mono::DataMemberInstance<double>>();
        const Mono::DataMemberInstance<double>& psi = i.get_value<Mono::DataMemberInstance<double>>();
        if (sfi.mScriptField.mFieldName == psi.mScriptField.mFieldName)
        {
          sfi.mData = psi.mData;
          SetFieldValue<double>(sfi.mData, sfi.mScriptField.mClassField);
        }
        break;
      }
      else if (f.is_type<Mono::DataMemberInstance<int64_t>>() && i.get_type() == f.get_type())
       {
         Mono::DataMemberInstance<int64_t>& sfi = f.get_value<Mono::DataMemberInstance<int64_t>>();
         const Mono::DataMemberInstance<int64_t>& psi = i.get_value<Mono::DataMemberInstance<int64_t>>();
         if (sfi.mScriptField.mFieldName == psi.mScriptField.mFieldName)
         {
           sfi.mData = psi.mData;
           SetFieldValue<int64_t>(sfi.mData, sfi.mScriptField.mClassField);
         }
         break;
       }
      else if (f.is_type<Mono::DataMemberInstance<uint16_t>>() && i.get_type() == f.get_type())
       {
         Mono::DataMemberInstance<uint16_t>& sfi = f.get_value<Mono::DataMemberInstance<uint16_t>>();
         const Mono::DataMemberInstance<uint16_t>& psi = i.get_value<Mono::DataMemberInstance<uint16_t>>();
         if (sfi.mScriptField.mFieldName == psi.mScriptField.mFieldName)
         {
           sfi.mData = psi.mData;
           SetFieldValue<uint16_t>(sfi.mData, sfi.mScriptField.mClassField);
         }
         break;
       }
      else if (f.is_type<Mono::DataMemberInstance<uint64_t>>() && i.get_type() == f.get_type())
       {
         Mono::DataMemberInstance<uint64_t>& sfi = f.get_value<Mono::DataMemberInstance<uint64_t>>();
         const Mono::DataMemberInstance<uint64_t>& psi = i.get_value<Mono::DataMemberInstance<uint64_t>>();
         if (sfi.mScriptField.mFieldName == psi.mScriptField.mFieldName)
         {
           sfi.mData = psi.mData;
           SetFieldValue<uint64_t>(sfi.mData, sfi.mScriptField.mClassField);
         }
         break;
       }
      else if (f.is_type<Mono::DataMemberInstance<std::string>>() && i.get_type() == f.get_type())
      {
        Mono::DataMemberInstance<std::string>& sfi = f.get_value<Mono::DataMemberInstance<std::string>>();
        const Mono::DataMemberInstance<std::string>& psi = i.get_value<Mono::DataMemberInstance<std::string>>();
        if (sfi.mScriptField.mFieldName == psi.mScriptField.mFieldName)
        {
          sfi.mData = psi.mData;
          mono_field_set_value(mClassInst, sfi.mScriptField.mClassField, STDToMonoString(sfi.mData));
        }
        break;
      }
      else if (f.is_type<Mono::DataMemberInstance<glm::vec3>>() && i.get_type() == f.get_type())
      {
        Mono::DataMemberInstance<glm::vec3>& sfi = f.get_value<Mono::DataMemberInstance<glm::vec3>>();
        const Mono::DataMemberInstance<glm::vec3>& psi = i.get_value<Mono::DataMemberInstance<glm::vec3>>();
        if (sfi.mScriptField.mFieldName == psi.mScriptField.mFieldName)
        {
          sfi.mData = psi.mData;
          SetFieldValue<glm::vec3>(sfi.mData, sfi.mScriptField.mClassField);
        }
        break;
        }
      else if (f.is_type<Mono::DataMemberInstance<glm::dvec3>>() && i.get_type() == f.get_type())
      {
        Mono::DataMemberInstance<glm::dvec3>& sfi = f.get_value<Mono::DataMemberInstance<glm::dvec3>>();
        const Mono::DataMemberInstance<glm::dvec3>& psi = i.get_value<Mono::DataMemberInstance<glm::dvec3>>();
        if (sfi.mScriptField.mFieldName == psi.mScriptField.mFieldName)
        {
          sfi.mData = psi.mData;
          SetFieldValue<glm::dvec3>(sfi.mData, sfi.mScriptField.mClassField);
        }
        break;
      }
      else if (f.is_type<Mono::DataMemberInstance<std::vector<int>>>() && i.get_type() == f.get_type())
      {
        Mono::DataMemberInstance<std::vector<int>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<int>>>();
        const Mono::DataMemberInstance<std::vector<int>>& psi = i.get_value<Mono::DataMemberInstance<std::vector<int>>>();
        if (sfi.mScriptField.mFieldName == psi.mScriptField.mFieldName)
        {
          sfi.mData = psi.mData;
          SetFieldValueArr<int>(sfi.mData, sfi.mScriptField.mClassField, sm->mAppDomain);
        }
        break;
      }
      else if (f.is_type<Mono::DataMemberInstance<std::vector<float>>>() && i.get_type() == f.get_type())
      {
        Mono::DataMemberInstance<std::vector<float>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<float>>>();
        const Mono::DataMemberInstance<std::vector<float>>& psi = i.get_value<Mono::DataMemberInstance<std::vector<float>>>();
        if (sfi.mScriptField.mFieldName == psi.mScriptField.mFieldName)
        {
          sfi.mData = psi.mData;
          SetFieldValueArr<float>(sfi.mData, sfi.mScriptField.mClassField, sm->mAppDomain);
        }
        break;
        }
      else if (f.is_type<Mono::DataMemberInstance<std::vector<double>>>() && i.get_type() == f.get_type())
      {
        Mono::DataMemberInstance<std::vector<double>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<double>>>();
        const Mono::DataMemberInstance<std::vector<double>>& psi = i.get_value<Mono::DataMemberInstance<std::vector<double>>>();
        if (sfi.mScriptField.mFieldName == psi.mScriptField.mFieldName)
        {
          sfi.mData = psi.mData;
          SetFieldValueArr<double>(sfi.mData, sfi.mScriptField.mClassField, sm->mAppDomain);
        }
        break;
        }
      else if (f.is_type<Mono::DataMemberInstance<std::vector<std::string>>>() && i.get_type() == f.get_type())
      {
        Mono::DataMemberInstance<std::vector<std::string>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<std::string>>>();
        const Mono::DataMemberInstance<std::vector<std::string>>& psi = i.get_value<Mono::DataMemberInstance<std::vector<std::string>>>();
        if (sfi.mScriptField.mFieldName == psi.mScriptField.mFieldName)
        {
          sfi.mData = psi.mData;
          std::vector<MonoString*> proxy{};
          for (const std::string& s : sfi.mData)
          {
            proxy.push_back(STDToMonoString(s));
          }
          SetFieldValueArr<MonoString*>(proxy, sfi.mScriptField.mClassField, sm->mAppDomain);
        }
        break;
      }
      else if (f.is_type<Mono::DataMemberInstance<std::vector<unsigned>>>() && i.get_type() == f.get_type())
      {
        Mono::DataMemberInstance<std::vector<unsigned>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<unsigned>>>();
        const Mono::DataMemberInstance<std::vector<unsigned>>& psi = i.get_value<Mono::DataMemberInstance<std::vector<unsigned>>>();
        if (sfi.mScriptField.mFieldName == psi.mScriptField.mFieldName)
        {
          sfi.mData = psi.mData;
          SetFieldValueArr<unsigned>(sfi.mData, sfi.mScriptField.mClassField, sm->mAppDomain);
        }
        break;
      }
      else if (f.is_type<Mono::DataMemberInstance<std::vector<MonoObject*>>>() && i.is_type<Mono::DataMemberInstance<std::vector<ScriptInstance>>>())
      {
        Mono::DataMemberInstance<std::vector<MonoObject*>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<MonoObject*>>>();
        const Mono::DataMemberInstance<std::vector<ScriptInstance>>& psi = i.get_value<Mono::DataMemberInstance<std::vector<ScriptInstance>>>();
        if (sfi.mScriptField.mFieldName == psi.mScriptField.mFieldName)
        {
          sfi.mData.clear();
          for (int i{ static_cast<int>(psi.mData.size()) - 1 }; i >= 0; --i)
          {
            sfi.mData.push_back(psi.mData[i].mClassInst);
          }
          SetFieldValueArr<MonoObject*>(sfi.mData, sfi.mScriptField.mClassField, sm->mAppDomain);
        }
        break;
        }
      else if (f.is_type<Mono::DataMemberInstance<ScriptInstance>>() && i.get_type() == f.get_type())
      {
        Mono::DataMemberInstance<ScriptInstance>& sfi = f.get_value<Mono::DataMemberInstance<ScriptInstance>>();
        const Mono::DataMemberInstance<ScriptInstance>& psi = i.get_value<Mono::DataMemberInstance<ScriptInstance>>();
        if (psi.mData.mClassInst)
        {
          if (sfi.mScriptField.mFieldName == psi.mScriptField.mFieldName)
          {
            sfi.mData = psi.mData;
            SetFieldValue<MonoObject>(sfi.mData.mClassInst, sfi.mScriptField.mClassField);
            break;
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
    if (f.is_type<Mono::DataMemberInstance<unsigned>>())
    {
       Mono::DataMemberInstance<unsigned>& sfi = f.get_value<Mono::DataMemberInstance<unsigned>>();
       sfi.mData = GetFieldValue<unsigned>(sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<bool>>())
    {
      Mono::DataMemberInstance<bool>& sfi = f.get_value<Mono::DataMemberInstance<bool>>();
      sfi.mData = GetFieldValue<bool>(sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<short>>())
    {
      Mono::DataMemberInstance<short>& sfi = f.get_value<Mono::DataMemberInstance<short>>();
      sfi.mData = GetFieldValue<short>(sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<int>>())
    {
      Mono::DataMemberInstance<int>& sfi = f.get_value<Mono::DataMemberInstance<int>>();
      sfi.mData = GetFieldValue<int>(sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<float>>())
    {
      Mono::DataMemberInstance<float>& sfi = f.get_value<Mono::DataMemberInstance<float>>();
      sfi.mData = GetFieldValue<float>(sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<double>>())
    {
      Mono::DataMemberInstance<double>& sfi = f.get_value<Mono::DataMemberInstance<double>>();
      sfi.mData = GetFieldValue<double>(sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<int64_t>>())
    {
      Mono::DataMemberInstance<int64_t>& sfi = f.get_value<Mono::DataMemberInstance<int64_t>>();
      sfi.mData = GetFieldValue<int64_t>(sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<uint16_t>>())
    {
      Mono::DataMemberInstance<uint16_t>& sfi = f.get_value<Mono::DataMemberInstance<uint16_t>>();
      sfi.mData = GetFieldValue<uint16_t>(sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<uint64_t>>())
    {
      Mono::DataMemberInstance<uint64_t>& sfi = f.get_value<Mono::DataMemberInstance<uint64_t>>();
      sfi.mData = GetFieldValue<uint64_t>(sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<std::string>>())
    {
      Mono::DataMemberInstance<std::string>& sfi = f.get_value<Mono::DataMemberInstance<std::string>>();
      sfi.mData = MonoStringToSTD(GetFieldValue<MonoString*>(sfi.mScriptField.mClassField));
    }
    else if (f.is_type<Mono::DataMemberInstance<glm::vec3>>())
    {
      Mono::DataMemberInstance<glm::vec3>& sfi = f.get_value<Mono::DataMemberInstance<glm::vec3>>();
      sfi.mData = GetFieldValue<glm::vec3>(sfi.mScriptField.mClassField);
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
    else if (f.is_type<Mono::DataMemberInstance<std::vector<float>>>())
    {
      Mono::DataMemberInstance<std::vector<float>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<float>>>();
      sfi.mData = GetFieldValueArr<float>(sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<std::vector<double>>>())
    {
      Mono::DataMemberInstance<std::vector<double>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<double>>>();
      sfi.mData = GetFieldValueArr<double>(sfi.mScriptField.mClassField);
    }
    else if (f.is_type<Mono::DataMemberInstance<std::vector<MonoObject*>>>())
    {
      Mono::DataMemberInstance<std::vector<MonoObject*>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<MonoObject*>>>();
      sfi.mData = GetFieldValueArr<MonoObject*>(sfi.mScriptField.mClassField);
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
    else if (f.is_type<Mono::DataMemberInstance<std::vector<unsigned>>>())
    {
      Mono::DataMemberInstance<std::vector<unsigned>>& sfi = f.get_value<Mono::DataMemberInstance<std::vector<unsigned>>>();
      sfi.mData = GetFieldValueArr<unsigned>(sfi.mScriptField.mClassField);
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


