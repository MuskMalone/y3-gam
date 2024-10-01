/*!*********************************************************************
\file   ScriptManager.cpp
\author han.q\@digipen.edu
\date   28-September-2024
\brief
  Script Manager Singleton in charge of initializing and cleaning the Mono.
  Provides function to retrieve C# class data
  Adds internal call into mono to allow C# to call functions defined in cpp

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "ScriptManager.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/tabledefs.h"
#include "mono/metadata/mono-debug.h"
#include "mono/metadata/threads.h"
#include <Prefabs/PrefabManager.h>

#include <filesystem>
#include <Serialization/Deserializer.h>
#include "Input/InputManager.h"
#include "Asset/AssetManager.h"


namespace Mono
{
  std::map<std::string, ScriptClassInfo> ScriptManager::mMonoClassMap{};
  std::shared_ptr<MonoAssembly> ScriptManager::mCoreAssembly{ nullptr };
  std::shared_ptr<MonoDomain> ScriptManager::mRootDomain{ nullptr };
  std::shared_ptr<MonoDomain> ScriptManager::mAppDomain{ nullptr };
  std::vector<std::string> ScriptManager::mAllScriptNames;
  std::string ScriptManager::mCoreAssFilePath{};
  std::string ScriptManager::mAppDomFilePath{};
  
  // Hot reload
  std::unique_ptr<filewatch::FileWatch<std::string>> ScriptManager::mCsProjWatcher{};
  std::unique_ptr<filewatch::FileWatch<std::string>> ScriptManager::mFileWatcher{};
  std::string ScriptManager::mScnfilePath{};
  std::string ScriptManager::mCsprojPath{};
  std::string ScriptManager::mBatfilePath{};
  bool ScriptManager::mAssemblyReloadPending{};
  bool ScriptManager::mCSReloadPending{};
  bool ScriptManager::mRebuildCS{};


  std::unordered_map<std::string, ScriptFieldType> ScriptManager::mScriptFieldTypeMap
  {
    { "System.Boolean", ScriptFieldType::BOOL },
    { "System.Char",    ScriptFieldType::CHAR },
    { "System.Int16",   ScriptFieldType::SHORT },
    { "System.Int32",   ScriptFieldType::INT },
    { "System.Single",  ScriptFieldType::FLOAT },
    { "System.Double",  ScriptFieldType::DOUBLE },
    { "System.Int64",   ScriptFieldType::LONG },
    { "System.UInt16",  ScriptFieldType::USHORT },
    { "System.UInt32",  ScriptFieldType::UINT },
    { "System.UInt64",  ScriptFieldType::ULONG },
    { "System.String",  ScriptFieldType::STRING },
    { "GoopScripts.Mono.Vec2<System.Single>", ScriptFieldType::VEC2 },
    { "GoopScripts.Mono.Vec3<System.Single>", ScriptFieldType::VEC3 },
    { "GoopScripts.Mono.Vec2<System.Double>", ScriptFieldType::DVEC2 },
    { "GoopScripts.Mono.Vec3<System.Double>", ScriptFieldType::DVEC3 },
    { "System.Int32[]", ScriptFieldType::INT_ARR },
    { "GoopScripts.Cards.CardBase.CardID[]", ScriptFieldType::UINT_ARR },
    {"System.String[]",ScriptFieldType::STRING_ARR}
  };
}

using namespace Mono;

// /*!*********************************************************************
//
//    Mono Init Codes
//
//************************************************************************/

void ScriptManager::InitMono()
{
  //Assets::AssetManager& assetManager{ Assets::AssetManager::GetInstance() };
  //std::ifstream file(assetManager.GetConfigData<std::string>("MonoAssemblyExeTest").c_str());
  std::ifstream file("../lib/mono/4.5/mscorlib.dll");
  if (file.good())
  {
    //mono_set_assemblies_path(assetManager.GetConfigData<std::string>("MonoAssemblyExe").c_str());
    mono_set_assemblies_path("../lib/mono/4.5/");
    file.close();
  }
  else
  {
    //mono_set_assemblies_path(assetManager.GetConfigData<std::string>("MonoAssembly").c_str());
    mono_set_assemblies_path("./4.5/");
  }

  //MonoDomain* rootDomain = mono_jit_init(assetManager.GetConfigData<std::string>("RootDomain").c_str());
  MonoDomain* rootDomain = mono_jit_init("ImaGEJITRuntime");
  if (rootDomain == nullptr)
  {
    Debug::DebugLogger::GetInstance().LogError("Unable to init Mono JIT", false);
    return;
  }

  //Store the root domain pointer
  mRootDomain = std::shared_ptr<MonoDomain>(rootDomain);

  //Create an App Domain
  //mAppDomFilePath = assetManager.GetConfigData<std::string>("AppDomain").c_str();
  mAppDomFilePath = "ImaGEAppDomain";
  LoadAppDomain();

  // Add the all the intercall calls
  AddInternalCalls();

  //Set the path for the core aseembly
  mCoreAssFilePath = "./Assets/Scripts/GoopScripts.dll";

  //Load the CSharpAssembly (dll file)
  LoadAssembly();

  //Load All the MonoClasses
  LoadAllMonoClass();

  try
  {
    mFileWatcher = std::make_unique<filewatch::FileWatch<std::string>>("./Assets/Scripts/ImaGE-script.dll", AssemblyFileSystemEvent);
    mAssemblyReloadPending = false;

    std::ifstream csfile("../ImaGE-script/ImaGE-script.csproj");
    if (csfile.good())
    {
      mCsprojPath = "../ImaGE-script/Source";
      mBatfilePath = "../ImaGE-script/reb.bat";
      mCsProjWatcher = std::make_unique < filewatch::FileWatch < std::string>>("../ImaGE-script/Source", CSReloadEvent);
      csfile.close();
    }
    else
    {
      mCsprojPath = "../../ImaGE-script/Source";
      mBatfilePath = "../../ImaGE-script/reb.bat";
      mCsProjWatcher = std::make_unique < filewatch::FileWatch < std::string>>("../../ImaGE-script/Source", CSReloadEvent);
    }
    mCSReloadPending = false;
  }
  catch (...)
  {
    Debug::DebugLogger::GetInstance().LogInfo("Not using Visual Studio, hotreload disabled");
  }

  // mScnfilePath = 
}

void ScriptManager::LoadAppDomain()
{
  mAppDomain = std::shared_ptr<MonoDomain>(mono_domain_create_appdomain(const_cast<char*>(mAppDomFilePath.c_str()), nullptr));
  mono_domain_set(mAppDomain.get(), true);
}

void ScriptManager::AddInternalCalls()
{
  // Input Functions
  mono_add_internal_call("GoopScripts.Mono.Utils::GetMouseScrollY", Input::InputManager::GetInstance().GetMouseScrollVert);
  mono_add_internal_call("GoopScripts.Mono.Utils::GetMouseScrollX", Input::InputManager::GetInstance().GetMouseScrollHor);
  mono_add_internal_call("GoopScripts.Mono.Utils::IsKeyTriggered", Input::InputManager::GetInstance().IsKeyTriggered);
  mono_add_internal_call("GoopScripts.Mono.Utils::IsKeyReleased", Input::InputManager::GetInstance().IsKeyReleased);
  mono_add_internal_call("GoopScripts.Mono.Utils::IsKeyPressed", Input::InputManager::GetInstance().IsKeyPressed);
  mono_add_internal_call("GoopScripts.Mono.Utils::IsKeyHeld", Input::InputManager::GetInstance().IsKeyHeld);
  mono_add_internal_call("GoopScripts.Mono.Utils::GetMousePos", Input::InputManager::GetInstance().GetMousePos);


  //// Get Functions
  //mono_add_internal_call("GoopScripts.Mono.Utils::GetPosition", GetPosition);
  //mono_add_internal_call("GoopScripts.Mono.Utils::GetWorldPosition", GetWorldPosition);
  //mono_add_internal_call("GoopScripts.Mono.Utils::GetRotation", GetRotation);
  //mono_add_internal_call("GoopScripts.Mono.Utils::GetScale", GetScale);

  //// Set Functions
  //mono_add_internal_call("GoopScripts.Mono.Utils::SetPosition", SetPosition);
  //mono_add_internal_call("GoopScripts.Mono.Utils::SetRotation", SetRotation);
  //mono_add_internal_call("GoopScripts.Mono.Utils::SetScale", SetScale);
}

void ScriptManager::LoadAssembly()
{
  mCoreAssembly = std::shared_ptr<MonoAssembly>(LoadCSharpAssembly(mCoreAssFilePath));
}

void ScriptManager::LoadAllMonoClass()
{
  mMonoClassMap.clear();
  mAllScriptNames.clear();
  MonoImage* image = mono_assembly_get_image(mCoreAssembly.get());
  const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
  int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

  for (int32_t i = 0; i < numTypes; i++)
  {
    uint32_t cols[MONO_TYPEDEF_SIZE];
    mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

    std::string classNameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
    std::string className = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
    if (classNameSpace.find("GoopScripts") != std::string::npos && classNameSpace.find("Properties") == std::string::npos)
    {
      //std::cout << classNameSpace << "::" << className << "\n";
      MonoClass* newClass = GetClassInAssembly(mCoreAssembly.get(), classNameSpace.c_str(), className.c_str());
      if (newClass)
      {
        ScriptClassInfo newScriptClassInfo{};
        newScriptClassInfo.mScriptClass = newClass;
        void* iterator = nullptr;
        while (MonoClassField* field = mono_class_get_fields(newClass, &iterator))
        {
          const char* fieldName = mono_field_get_name(field);
          uint32_t flags = mono_field_get_flags(field);
          if (flags & FIELD_ATTRIBUTE_PUBLIC)
          {
            MonoType* type = mono_field_get_type(field);
            ScriptFieldType fieldType = MonoTypeToScriptFieldType(type);
            std::string typeName = mono_type_get_name(type);
            //std::cout << fieldName << "\n";
            newScriptClassInfo.mScriptFieldMap[fieldName] = { fieldType, fieldName, field };
          }
        }
        mMonoClassMap[className] = newScriptClassInfo;
        MonoMethod* ctor = mono_class_get_method_from_name(newClass, ".ctor", 0);
        MonoMethod* ctor2 = mono_class_get_method_from_name(newClass, ".ctor", 1);
        if (ctor || ctor2)
        {
          mAllScriptNames.push_back(className);
        }

      }
    }
  }
  std::sort(mAllScriptNames.begin(), mAllScriptNames.end());
}

// this function i nscenemanaegere
void ScriptManager::ReloadAllScripts()
{
  auto gsm = &GE::GSM::GameStateManager::GetInstance();
  Assets::AssetManager& assetManager{ Assets::AssetManager::GetInstance() };
  static auto& ecs = GE::ECS::EntityComponentSystem::GetInstance();
  std::string scnName = gsm->GetCurrentScene();
  mScnfilePath = assetManager.GetConfigData<std::string>("Assets Dir") + "Scenes/" + scnName + ".scn";
  //std::cout << "SCENE FILE PATH: " << mScnfilePath << "\n";
  //std::cout << "Reload All Scripts\n";


  for (GE::ECS::Entity const& entity : ecs.GetEntities())
  {

    if (ecs.HasComponent<GE::Component::Scripts>(entity))
    {
      GE::Component::Scripts* scripts = ecs.GetComponent<GE::Component::Scripts>(entity);
      //for (auto& script : scripts->mScriptList)
      //{
      //  //script.ReloadScript();
      //}
      //for (auto sp : scripts->mScriptList)
      //{
      //  std::cout << entity << ": " << sp.mScriptName << " deleted\n";
      //}
      scripts->mScriptList.clear();

    }
  }

  GE::Prefabs::PrefabManager::GetInstance().ReloadPrefabs();
  auto newScriptMap{ GE::Serialization::Deserializer::DeserializeSceneScripts(mScnfilePath) };
  for (auto& s : newScriptMap)
  {
    if (ecs.HasComponent<GE::Component::Scripts>(s.first))
    {
      GE::Component::Scripts* scripts = ecs.GetComponent<GE::Component::Scripts>(s.first);
      //#ifdef _DEBUG
      //      std::cout << s.first << ": " << s.second.size() << " adding\n";
      //#endif
      for (auto& si : s.second)
      {
        scripts->mScriptList.push_back(si);
      }
    }
  }


}

void ScriptManager::AssemblyFileSystemEvent(const std::string& path, const filewatch::Event change_type)
{

  if (!mAssemblyReloadPending && change_type == filewatch::Event::modified)
  {
    mAssemblyReloadPending = true;
    auto gsm = &GE::GSM::GameStateManager::GetInstance();
    //#ifdef _DEBUG
    //    std::cout << "AddCmd to main thread\n";
    //#endif
    gsm->SubmitToMainThread([]()
      {
        mFileWatcher.reset();
        ReloadAssembly();
      });
  }
}

void ScriptManager::CSReloadEvent(const std::string& path, const filewatch::Event change_type)
{
  if (!mCSReloadPending && change_type == filewatch::Event::modified && !mRebuildCS)
  {
#ifdef _DEBUG
    std::cout << "RELOAD CS\n";
#endif
    mCSReloadPending = true;
    auto gsm = &GE::GSM::GameStateManager::GetInstance();
    //#ifdef _DEBUG
    //    std::cout << "Lets rebuild\n";
    //#endif
    mRebuildCS = true;
    gsm->SubmitToMainThread([]()
      {
        mCsProjWatcher.reset();
        RebuildCS();
      });
  }
}


// Function to get Visual Studio version
std::string GetVisualStudioVersion() {
    std::string VSver{};
  #ifdef _MSC_VER
    VSver = (_MSC_VER >= 1930) ? "2022" : (_MSC_VER >= 1920) ? "2019" : (_MSC_VER >= 1910) ? "2017" : (_MSC_VER >= 1900) ? "2015" : (_MSC_VER >= 1900) ? "2013" : "2012";
  #else
    GE::Debug::ErrorLogger::GetInstance().LogMessage("Not using Visual Studio, hotreload disabled");
  #endif
  #ifdef _DEBUG
    std::cout << VSver << "::VS VER\n";
  #endif
    return VSver;
}

void ScriptManager::RebuildCS()
{

#ifdef _DEBUG
  std::cout << "REBUILDCS\n";
#endif
  mCSReloadPending = false;
  if (mRebuildCS)
  {
    mRebuildCS = false;
    std::string const csbat = std::filesystem::absolute(mBatfilePath).string();
    std::string vsVer = GetVisualStudioVersion();

    std::string arguments = vsVer;
#ifdef _DEBUG
    arguments += " Debug";
#else
    arguments += " Release";
#endif

    std::string cdCMD = "\"" + csbat + "\" ";
    std::string command = cdCMD + arguments;
    //#ifdef _DEBUG
    //    std::cout << command.c_str() << "\n";
    //#endif
    int result = system(command.c_str());
    mCsProjWatcher = std::make_unique < filewatch::FileWatch < std::string>>(mCsprojPath, CSReloadEvent);
    mCSReloadPending = false;

    if (result == 0) {
#ifdef _DEBUG
      std::cout << "RUN Successfuly\n";
#endif
    }
    else {
#ifdef _DEBUG
      std::cout << "DIDNT RUN Successfuly\n";
#endif
    }
  }
}



void ScriptManager::ReloadAssembly()
{
#ifdef _DEBUG
  std::cout << "ASSReload\n";
#endif
  mono_domain_set(mono_get_root_domain(), false);
  mono_domain_unload(mAppDomain.get());
  mMonoClassMap.clear();
  mAllScriptNames.clear();

  LoadAppDomain();
  Assets::AssetManager& assetManager{ Assets::AssetManager::GetInstance() };
  mFileWatcher = std::make_unique < filewatch::FileWatch < std::string>>(assetManager.GetConfigData<std::string>("CAssembly"), AssemblyFileSystemEvent);
  mAssemblyReloadPending = false;

  ReloadScripts();

}

void ScriptManager::ReloadScripts()
{
  LoadAssembly();
  AddInternalCalls();
  LoadAllMonoClass();
  ReloadAllScripts();
}

MonoAssembly* ScriptManager::GetMonoAssembly()
{
  return mCoreAssembly.get();
}

MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className)
{
  MonoImage* image = mono_assembly_get_image(assembly);
  MonoClass* klass = mono_class_from_name(image, namespaceName, className);

  if (klass == nullptr)
  {
    Debug::DebugLogger::GetInstance().LogWarning("Unable to access c# class " + std::string(className), false);
  }
  return klass;
}

MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath)
{
  uint32_t fileSize = 0;
  char* fileData;
  try
  {
    fileData = ReadBytes(assemblyPath, &fileSize);
  }
  catch (Debug::ExceptionBase& e)
  {
    e.LogSource();
    e.Log();
    throw Debug::Exception<ScriptManager>(Debug::LVL_ERROR, "Read file fail", __FUNCTION__, __LINE__);
  }

  MonoImageOpenStatus status;
  MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);
  if (status != MONO_IMAGE_OK)
  {
    const char* errorMessage = mono_image_strerror(status);
    delete[] fileData;
    throw Debug::Exception<ScriptManager>(Debug::LVL_ERROR, errorMessage, __FUNCTION__, __LINE__);
  }

  MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
  mono_image_close(image);

  //Free the memory from Read Bytes
  delete[] fileData;
  return assembly;
}

char* ReadBytes(const std::string& filepath, uint32_t* outSize)
{
  std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

  if (!stream)
  {
    throw Debug::Exception<ScriptManager>(Debug::LVL_ERROR, "Unable to read for file: " + filepath, __FUNCTION__, __LINE__);
  }

  std::streampos end = stream.tellg();
  stream.seekg(0, std::ios::beg);
  uint32_t size = static_cast<uint32_t>(end - stream.tellg());

  if (size == 0)
  {
    throw Debug::Exception<ScriptManager>(Debug::LVL_ERROR, "The data file: " + filepath + " is empty", __FUNCTION__, __LINE__);
  }

  char* buffer = new char[size];
  stream.read((char*)buffer, size);
  stream.close();

  *outSize = size;
  return buffer;
}

ScriptFieldType ScriptManager::MonoTypeToScriptFieldType(MonoType* monoType)
{
  std::string typeName = mono_type_get_name(monoType);
  //std::cout << typeName << "::";
  auto it = mScriptFieldTypeMap.find(typeName);
  if (it == mScriptFieldTypeMap.end())
  {
    //GE::Debug::ErrorLogger::GetInstance().LogWarning("Unable to access c# field type" + typeName, false);
    return ScriptFieldType::S_VOID;
  }

  return it->second;
}


/*!**********************************************************************
*																																			  *
*								           Mono Shutdown Code			          						*
*																																			  *
************************************************************************/

ScriptManager::~ScriptManager()
{
  if (mRootDomain)
  {
    mono_domain_set(mono_get_root_domain(), false);

    mono_domain_unload(mAppDomain.get());
    mAppDomain = nullptr;

    mono_jit_cleanup(mRootDomain.get());
    mRootDomain = nullptr;
  }

}


/*!**********************************************************************
*																																			  *
*								            Debugging Code			          						  *
*																																			  *
************************************************************************/




/*!**********************************************************************
*																																			  *
*								            Internal Calls			          						  *
*																																			  *
************************************************************************/


void SetPosition(ECS::Entity::EntityID entity, glm::dvec3 posAdjustment)
{
  Component::Transform& oldTransform = ECS::Entity(entity).GetComponent<Component::Transform>();
  oldTransform.localPos = posAdjustment;
}

void SetScale(ECS::Entity::EntityID entity, glm::dvec3 scaleAdjustment)
{
  Component::Transform& oldTransform = ECS::Entity(entity).GetComponent<Component::Transform>();
  oldTransform.localScale = scaleAdjustment;
}

void SetRotation(ECS::Entity::EntityID entity, glm::dvec3 rotAdjustment)
{
  Component::Transform& oldTransform = ECS::Entity(entity).GetComponent<Component::Transform>();
  oldTransform.localRot = rotAdjustment;
}

glm::dvec3 GetPosition(ECS::Entity::EntityID entity)
{
  return ECS::Entity(entity).GetComponent<Component::Transform>().localPos;
}

glm::dvec3 GetScale(ECS::Entity::EntityID entity)
{
  return ECS::Entity(entity).GetComponent<Component::Transform>().localScale;
}

glm::dvec3 GetRotation(ECS::Entity::EntityID entity)
{
  return ECS::Entity(entity).GetComponent<Component::Transform>().localRot;
}