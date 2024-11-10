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
#include <Core/Systems/TransformSystem/TransformHelpers.h>
#include <Scenes/SceneManager.h>

#include <filesystem>
#include <Core/Components/Components.h>
#include <Serialization/Deserializer.h>
#include <Scripting/ScriptInstance.h>
#include "Input/InputManager.h"
#include "Asset/AssetManager.h"
#include "Physics/PhysicsSystem.h"
#define DEBUG_MONO
namespace Mono
{
  std::map<std::string, ScriptClassInfo> ScriptManager::mMonoClassMap{};
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

  std::unordered_map<ScriptFieldType,std::string> ScriptManager::mRevClassMap{};
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
    { "IGE.Utils.Vec2<System.Single>", ScriptFieldType::VEC2 },
    { "IGE.Utils.Vec3<System.Single>", ScriptFieldType::VEC3 },
    { "IGE.Utils.Vec2<System.Double>", ScriptFieldType::DVEC2 },
    { "IGE.Utils.Vec3<System.Double>", ScriptFieldType::DVEC3 },
    { "System.Int32[]", ScriptFieldType::INT_ARR },
    { "System.String[]",ScriptFieldType::STRING_ARR},
    { "Entity",ScriptFieldType::ENTITY},
    { "Inside",ScriptFieldType::INSIDE},
    { "InsideB",ScriptFieldType::INSIDEB}
  };
}

using namespace Mono;

/*!**********************************************************************
*																																			  *
*								            Mono Init Code			          						  *
*																																			  *
************************************************************************/

ScriptManager::ScriptManager()
{
  //Assets::AssetManager& assetManager{ Assets::AssetManager::GetInstance() };
  //std::ifstream file(assetManager.GetConfigData<std::string>("MonoAssemblyExeTest").c_str());
  std::ifstream file("../ImaGE-Core/source/External/mono/4.5/mscorlib.dll");
  if (file.good())
  {
    //mono_set_assemblies_path(assetManager.GetConfigData<std::string>("MonoAssemblyExe").c_str());
    mono_set_assemblies_path("../ImaGE-Core/source/External/mono/4.5/");
    file.close();
  }
  else
  {
    //mono_set_assemblies_path(assetManager.GetConfigData<std::string>("MonoAssembly").c_str());
    mono_set_assemblies_path("./4.5/");
  }
  std::cout << "Finish load assembly file\n";
  //MonoDomain* rootDomain = mono_jit_init(assetManager.GetConfigData<std::string>("RootDomain").c_str());
  MonoDomain* rootDomain = mono_jit_init("ImaGEJITRuntime");
  if (rootDomain == nullptr)
  {
    Debug::DebugLogger::GetInstance().LogError("Unable to init Mono JIT", false);
    return;
  }

  //Store the root domain pointer
  mRootDomain = std::shared_ptr<MonoDomain>(rootDomain, mono_jit_cleanup);

  //Create an App Domain
  //mAppDomFilePath = assetManager.GetConfigData<std::string>("AppDomain").c_str();
  mAppDomFilePath = "ImaGEAppDomain";
  LoadAppDomain();

  // Add the all the intercall calls
  AddInternalCalls();

  //Set the path for the core aseembly
  mCoreAssFilePath = "../Assets/Scripts/ImaGE-Script.dll";

  //Load All the MonoClasses
  LoadAllMonoClass();

  try
  {
    mFileWatcher = std::make_unique<filewatch::FileWatch<std::string>>("../Assets/Scripts/ImaGE-Script.dll", AssemblyFileSystemEvent);
    mAssemblyReloadPending = false;

    std::ifstream csfile("../ImaGE-Script/ImaGE-Script.csproj");
    if (csfile.good())
    {
      mCsprojPath = "../ImaGE-Script/Source";
      mBatfilePath = "../ImaGE-Script/reb.bat";
      mCsProjWatcher = std::make_unique < filewatch::FileWatch < std::string>>("../ImaGE-Script/Source", CSReloadEvent);
      csfile.close();
    }
    else
    {
      mCsprojPath = "../../ImaGE-Script/Source";
      mBatfilePath = "../../ImaGE-Script/reb.bat";
      mCsProjWatcher = std::make_unique < filewatch::FileWatch < std::string>>("../../ImaGE-Script/Source", CSReloadEvent);
    }
    mCSReloadPending = false;
  }
  catch (...)
  {
    Debug::DebugLogger::GetInstance().LogInfo("Not using Visual Studio, hotreload disabled");
  }


}

void ScriptManager::LoadAppDomain()
{
  mAppDomain = std::shared_ptr<MonoDomain>(mono_domain_create_appdomain(const_cast<char*>(mAppDomFilePath.c_str()), nullptr), mono_domain_unload);
  mono_domain_set(mAppDomain.get(), true);
}

#define ADD_INTERNAL_CALL(func) mono_add_internal_call("IGE.Utils.InternalCalls::"#func, Mono::func);
#define ADD_CLASS_INTERNAL_CALL(func, instance) mono_add_internal_call("IGE.Utils.InternalCalls::"#func, instance.func);

void ScriptManager::AddInternalCalls()
{
  // Input Functions
  ADD_CLASS_INTERNAL_CALL(IsKeyTriggered, Input::InputManager::GetInstance());
  //ADD_CLASS_INTERNAL_CALL(IsKeyReleased, Input::InputManager::GetInstance());
  //ADD_CLASS_INTERNAL_CALL(IsKeyPressed, Input::InputManager::GetInstance());
  ADD_CLASS_INTERNAL_CALL(IsKeyHeld, Input::InputManager::GetInstance()); 
  ADD_CLASS_INTERNAL_CALL(GetInputString, Input::InputManager::GetInstance());
  ADD_CLASS_INTERNAL_CALL(AnyKeyDown, Input::InputManager::GetInstance());
  ADD_INTERNAL_CALL(GetMousePos);
  ADD_INTERNAL_CALL(GetMouseDelta);

  //// Get Functions
  ADD_INTERNAL_CALL(GetWorldPosition);
  ADD_INTERNAL_CALL(GetPosition);
  ADD_INTERNAL_CALL(GetWorldRotation);
  ADD_INTERNAL_CALL(GetRotation);
  ADD_INTERNAL_CALL(GetWorldScale);
  ADD_INTERNAL_CALL(GetScale);
  ADD_INTERNAL_CALL(GetColliderScale);
  ADD_INTERNAL_CALL(GetVelocity);

  //// Set Functions
  ADD_INTERNAL_CALL(SetWorldPosition);
  ADD_INTERNAL_CALL(SetPosition);
  ADD_INTERNAL_CALL(SetRotation);
  ADD_INTERNAL_CALL(SetWorldRotation);
  ADD_INTERNAL_CALL(SetWorldScale);
  ADD_INTERNAL_CALL(MoveCharacter);

  //Debug Functions
  ADD_INTERNAL_CALL(Log);
  ADD_INTERNAL_CALL(LogWarning);
  ADD_INTERNAL_CALL(LogError);
  ADD_INTERNAL_CALL(LogCritical);

  //ADD_INTERNAL_CALL(Anykeydow)
  ADD_INTERNAL_CALL(GetAxis);
  ADD_INTERNAL_CALL(GetDeltaTime);
  ADD_INTERNAL_CALL(MoveCharacter);
  ADD_INTERNAL_CALL(IsGrounded);
  ADD_INTERNAL_CALL(GetTag);
  ADD_INTERNAL_CALL(FindScript);
  ADD_INTERNAL_CALL(DestroyEntity);
  ADD_INTERNAL_CALL(DestroyScript);
  ADD_INTERNAL_CALL(SetActive);

  // Utility Functions
  ADD_INTERNAL_CALL(Raycast);
  ADD_INTERNAL_CALL(PlaySound);
  ADD_INTERNAL_CALL(GetLayerName);
}

void ScriptManager::LoadAllMonoClass()
{
  mMonoClassMap.clear();
  mAllScriptNames.clear();
  MonoAssembly* coreAssembly = LoadCSharpAssembly(mCoreAssFilePath);
  MonoImage* image = mono_assembly_get_image(coreAssembly);
  const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
  int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

  for (int32_t i = 0; i < numTypes; i++)
  {
    uint32_t cols[MONO_TYPEDEF_SIZE];
    mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

    std::string classNameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
    std::string className = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
    if (className.find("<") == std::string::npos && classNameSpace.find("Utils") == std::string::npos)
    {
#ifdef DEBUG_MONO
      std::cout << classNameSpace << "::" << className << "\n";
      std::cout << "----------------------------------\n";
#endif 
      MonoClass* newClass = GetClassInAssembly(coreAssembly, classNameSpace.c_str(), className.c_str());
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
#ifdef DEBUG_MONO
            std::cout << typeName <<  "::" << fieldName  << "\n";
#endif 
            newScriptClassInfo.mScriptFieldMap[fieldName] = { fieldType, fieldName, field };
          }
        }
        MonoMethod* ctor = mono_class_get_method_from_name(newClass, ".ctor", 0);
        if (ctor)
        {
          mMonoClassMap[className] = newScriptClassInfo;
        }
#ifdef DEBUG_MONO
        else
          std::cout << classNameSpace + '.' + className << "\n";
#endif // DEBUG
        if (className.find("Entity") == std::string::npos &&  IsMonoBehaviourclass(newClass)) // If the class is not the base entity class and inherits from it, we will add it to the list for inspector
        {          
          mAllScriptNames.push_back(className);
        }
        
      }
#ifdef DEBUG_MONO
      std::cout << "----------------------------------\n\n";
#endif 
    }
  }
  std::sort(mAllScriptNames.begin(), mAllScriptNames.end());
  for (const auto& pair : ScriptManager::mScriptFieldTypeMap) {
    mRevClassMap[pair.second] = pair.first;
  }
}

bool ScriptManager::IsMonoBehaviourclass(MonoClass* mc)
{
  bool isMonoBeh{ false };
  MonoClass* parent = mc;
  while (parent)
  {
    if (std::string(mono_class_get_name(parent)).find("Entity") != std::string::npos)
    {
      isMonoBeh = true;
      break;
    }
    parent = mono_class_get_parent(parent);
  }
  return isMonoBeh;
}

MonoClass* Mono::GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className)
{
  MonoImage* image = mono_assembly_get_image(assembly);
  MonoClass* klass = mono_class_from_name(image, namespaceName, className);

  if (klass == nullptr)
  {
    Debug::DebugLogger::GetInstance().LogWarning("Unable to access c# class " + std::string(className), false);
  }
  return klass;
}

MonoAssembly* Mono::LoadCSharpAssembly(const std::string& assemblyPath)
{
  uint32_t fileSize = 0;
  char* fileData;
  try
  {
    fileData = ReadBytes(assemblyPath, &fileSize);
  }
  catch (Debug::ExceptionBase& e)
  {
    std::filesystem::path currentPath = std::filesystem::current_path();

    // Print the current directory
    std::cout << "Current directory: " << currentPath << std::endl;
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

  MonoAssembly* assembly = mono_assembly_load_from_full(image, "../Assets/Scripts/ImaGE-Script.dll", &status, 0);
  mono_image_close(image);

  //Free the memory from Read Bytes
  delete[] fileData;
  return assembly;
}

char* Mono::ReadBytes(const std::string& filepath, uint32_t* outSize)
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
*								          Hot Reload Function			          						*
*																																			  *
************************************************************************/


void ScriptManager::ReloadAllScripts()
{
  //auto gsm = &GE::GSM::GameStateManager::GetInstance();
  //Assets::AssetManager& assetManager{ Assets::AssetManager::GetInstance() };
  //static auto& ecs = GE::ECS::EntityComponentSystem::GetInstance();
  //std::string scnName = gsm->GetCurrentScene();
  //m_scnfilePath = assetManager.GetConfigData<std::string>("Assets Dir") + "Scenes/" + scnName + ".scn";
  ////std::cout << "SCENE FILE PATH: " << m_scnfilePath << "\n";
  ////std::cout << "Reload All Scripts\n";
  Scenes::SceneManager::GetInstance().ReloadScene();

  //for (GE::ECS::Entity const& entity : ecs.GetEntities())
  //{

  //  if (ecs.HasComponent<GE::Component::Scripts>(entity))
  //  {
  //    GE::Component::Scripts* scripts = ecs.GetComponent<GE::Component::Scripts>(entity);
  //    //for (auto& script : scripts->m_scriptList)
  //    //{
  //    //  //script.ReloadScript();
  //    //}
  //    //for (auto sp : scripts->m_scriptList)
  //    //{
  //    //  std::cout << entity << ": " << sp.m_scriptName << " deleted\n";
  //    //}
  //    scripts->m_scriptList.clear();

  //  }
  //}

  //GE::Prefabs::PrefabManager::GetInstance().ReloadPrefabs();
  //auto newScriptMap{ GE::Serialization::Deserializer::DeserializeSceneScripts(m_scnfilePath) };
  //for (auto& s : newScriptMap)
  //{
  //  if (ecs.HasComponent<GE::Component::Scripts>(s.first))
  //  {
  //    GE::Component::Scripts* scripts = ecs.GetComponent<GE::Component::Scripts>(s.first);
  //    //#ifdef _DEBUG
  //    //      std::cout << s.first << ": " << s.second.size() << " adding\n";
  //    //#endif
  //    for (auto& si : s.second)
  //    {
  //      scripts->m_scriptList.push_back(si);
  //    }
  //  }
  //}
}

void ScriptManager::AssemblyFileSystemEvent(const std::string& path, const filewatch::Event change_type)
{

  if (!mAssemblyReloadPending && change_type == filewatch::Event::modified)
  {
    mAssemblyReloadPending = true;
    auto sm = &Scenes::SceneManager::GetInstance();
    //#ifdef _DEBUG
    //    std::cout << "AddCmd to main thread\n";
    //#endif
    sm->SubmitToMainThread([]()
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
    auto sm = &Scenes::SceneManager::GetInstance();
    //#ifdef _DEBUG
    //    std::cout << "Lets rebuild\n";
    //#endif
    mRebuildCS = true;
    sm->SubmitToMainThread([]()
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
  mAppDomain.reset();
  mMonoClassMap.clear();
  mAllScriptNames.clear();

  LoadAppDomain();
  //Assets::AssetManager& assetManager{ Assets::AssetManager::GetInstance() };
  mFileWatcher = std::make_unique < filewatch::FileWatch < std::string>>("../Assets/Scripts/ImaGE-Script.dll", AssemblyFileSystemEvent);
  mAssemblyReloadPending = false;

  ReloadScripts();

}

void ScriptManager::ReloadScripts()
{
  AddInternalCalls();
  LoadAllMonoClass();
  ReloadAllScripts();
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

 
    mAppDomain.reset();

  
    //mono_jit_cleanup(mRootDomain.get());
    mRootDomain.reset();
  }

}



/*!**********************************************************************
*																																			  *
*								    Functions to instantiate script	          				  *
*																																			  *
************************************************************************/

MonoObject* Mono::ScriptManager::InstantiateClass(const char* className, std::vector<void*> arg)
{
  if (mMonoClassMap.find(className) != mMonoClassMap.end())
  {
    MonoClass* currClass = mMonoClassMap[className].mScriptClass;
    if (!currClass)
    {
      throw Debug::Exception<ScriptManager>(Debug::LVL_CRITICAL, Msg("Unable to fetch script: " + std::string(className)));
    }

    MonoObject* classInstance = mono_object_new(mAppDomain.get(), currClass);  //Get a reference to the class we want to instantiate


    if (classInstance == nullptr)
    {
      throw Debug::Exception<ScriptManager>(Debug::LVL_ERROR, "Failed to Allocate memory for class " + std::string(className), __FUNCTION__, __LINE__);
    }

    //Init the class through non-default constructor
    MonoMethod* classCtor = mono_class_get_method_from_name(currClass, ".ctor", static_cast<int>(arg.size()));
    if (!classCtor)
    {
      classCtor = mono_class_get_method_from_name(currClass, ".ctor", static_cast<int>(arg.size()));
    }
    mono_runtime_invoke(classCtor, classInstance, arg.data(), nullptr);


    if (classInstance == nullptr) {
      throw Debug::Exception<ScriptManager>(Debug::LVL_ERROR, "Failed to Create the class object with non-default constructor: " + std::string(className), __FUNCTION__, __LINE__);
    }
    return classInstance;
  }

  throw Debug::Exception<ScriptManager>(Debug::LVL_ERROR, "Failed to locate class in map" + std::string(className), __FUNCTION__, __LINE__);
}

MonoClass* Mono::ScriptManager::GetScriptClass(std::string className)
{
  return mMonoClassMap[className].mScriptClass;
}


ScriptClassInfo Mono::ScriptManager::GetScriptClassInfo(std::string className)
{
  return mMonoClassMap[className];
}

ScriptFieldInfo Mono::ScriptManager::GetScriptField(std::string className, std::string fieldName)
{
  return mMonoClassMap[className].mScriptFieldMap[fieldName];
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

void Mono::SetWorldPosition(ECS::Entity::EntityID entity, glm::vec3 posAdjustment) {
  Component::Transform& trans{ ECS::Entity(entity).GetComponent<Component::Transform>() };
  trans.worldPos = posAdjustment;
  trans.modified = true;
}

// @TODO: needs testing
void Mono::SetPosition(ECS::Entity::EntityID entity, glm::vec3 newPosition) {
  Component::Transform& trans{ ECS::Entity(entity).GetComponent<Component::Transform>() };
  trans.position = newPosition;
  TransformHelpers::UpdateWorldTransform(entity);
}

void Mono::SetWorldScale(ECS::Entity::EntityID entity, glm::vec3 scaleAdjustment) {
  Component::Transform& trans{ ECS::Entity(entity).GetComponent<Component::Transform>() };
  trans.scale = scaleAdjustment;
  TransformHelpers::UpdateWorldTransform(entity);
}

glm::vec3 Mono::GetScale(ECS::Entity::EntityID entity)
{
    Component::Transform& trans{ ECS::Entity(entity).GetComponent<Component::Transform>() };
    return trans.scale;
}

glm::vec3 Mono::GetColliderScale(ECS::Entity::EntityID e)
{
    ECS::Entity entity(e);
    if (entity.HasComponent<Component::BoxCollider>()) {
        auto& collider{ entity.GetComponent<Component::BoxCollider>() };
        return glm::vec3{ collider.scale.x, collider.scale.y, collider.scale.z };
    }
    return glm::vec3{};
}

glm::quat Mono::GetWorldRotation(ECS::Entity::EntityID entity)
{
  Component::Transform& trans{ ECS::Entity(entity).GetComponent<Component::Transform>() };
  return trans.worldRot;

  // need to use quaternions
}

void Mono::SetWorldRotation(ECS::Entity::EntityID entity, glm::quat rotAdjustment)
{
  Component::Transform& trans{ ECS::Entity(entity).GetComponent<Component::Transform>() };
  trans.worldRot = rotAdjustment;
  trans.modified = true;
  // need to use quaternions
}

glm::quat Mono::GetRotation(ECS::Entity::EntityID entity)
{
  Component::Transform& trans{ ECS::Entity(entity).GetComponent<Component::Transform>() };
  return trans.rotation;
  // need to use quaternions
}

void Mono::SetRotation(ECS::Entity::EntityID entity, glm::quat rotAdjustment)
{
  Component::Transform& trans{ ECS::Entity(entity).GetComponent<Component::Transform>() };
  trans.rotation = rotAdjustment;
  trans.modified = true;
  // need to use quaternions
}

glm::vec3 Mono::GetWorldPosition(ECS::Entity::EntityID entity)
{
  return ECS::Entity(entity).GetComponent<Component::Transform>().worldPos;
}

glm::vec3 Mono::GetPosition(ECS::Entity::EntityID entity)
{
  return ECS::Entity(entity).GetComponent<Component::Transform>().position;
}

glm::vec3 Mono::GetWorldScale(ECS::Entity::EntityID entity)
{
  return ECS::Entity(entity).GetComponent<Component::Transform>().scale;
}

MonoString* Mono::GetTag(ECS::Entity::EntityID entity)
{
  /*
  if (ECS::Entity(entity).HasComponent<Component::Tag>())
    return STDToMonoString(ECS::Entity(entity).GetComponent<Component::Tag>().tag);
  else
    return STDToMonoString("");
  */

  // @TODO: TEMP
  return STDToMonoString(ECS::Entity(entity).GetComponent<Component::Tag>().tag);
}

void  Mono::Log(MonoString*s)
{
  std::string msg{ MonoStringToSTD(s) };
  Debug::DebugLogger::GetInstance().LogInfo(msg);
}

void  Mono::LogWarning(MonoString* s)
{
  std::string msg{ MonoStringToSTD(s) };
  Debug::DebugLogger::GetInstance().LogWarning(msg);
}

void  Mono::LogError(MonoString* s)
{
  std::string msg{ MonoStringToSTD(s) };
  Debug::DebugLogger::GetInstance().LogError(msg);
}

void  Mono::LogCritical(MonoString* s)
{
  std::string msg{ MonoStringToSTD(s) };
  Debug::DebugLogger::GetInstance().LogCritical(msg);
}

float  Mono::GetAxis(MonoString* s)
{
  std::string msg{ MonoStringToSTD(s) };
  return Input::InputManager::GetInstance().GetAxis(msg);
}

void Mono::MoveCharacter(ECS::Entity::EntityID entity, glm::vec3 dVec)
{
  if (ECS::Entity(entity).HasComponent<Component::RigidBody>())
  {
    //std::cout << "Move: " << dVec.x << "," << dVec.y << "," << dVec.z << "\n";
    ECS::Entity(entity).GetComponent<Component::RigidBody>().velocity.x = dVec.x;
    if(dVec.y == 20.f)
      ECS::Entity(entity).GetComponent<Component::RigidBody>().velocity.y = dVec.y;
    ECS::Entity(entity).GetComponent<Component::RigidBody>().velocity.z = dVec.z;
     
    IGE::Physics::PhysicsSystem::GetInstance().get()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::VELOCITY);

  }
    
}

glm::vec3 Mono::GetMouseDelta()
{
  //std::cout << Input::InputManager::GetInstance().GetMousePos() << "\n";
  return glm::vec3(Input::InputManager::GetInstance().GetMouseDelta(),0);
}

glm::vec3 Mono::GetMousePos()
{
  return glm::vec3(Input::InputManager::GetInstance().GetMousePos(), 0);
}

bool  Mono::IsGrounded(ECS::Entity::EntityID entity)
{
  if (ECS::Entity(entity).HasComponent<Component::RigidBody>())
  {
    //std::cout << ECS::Entity(entity).GetComponent<Component::RigidBody>().velocity.y << "\n";
    return(ECS::Entity(entity).GetComponent<Component::RigidBody>().velocity.y <= 0 && ECS::Entity(entity).GetComponent<Component::RigidBody>().velocity.y >= -0.005);
  }
    
  return false;
}

ECS::Entity::EntityID Mono::Raycast(glm::vec3 start, glm::vec3 end)
{
    IGE::Physics::RaycastHit hit{};
    IGE::Physics::PhysicsSystem::GetInstance()->RayCastSingular(start, end, hit);
    ECS::Entity::EntityID out {hit.entity.GetEntityID()};
    return out;
}

void Mono::PlaySound(ECS::Entity::EntityID e, MonoString* s)
{
    std::string name{ MonoStringToSTD(s) };
    ECS::Entity entity{ e };
    entity.GetComponent<Component::AudioSource>().PlaySound(name);
}

glm::vec3 Mono::GetVelocity(ECS::Entity::EntityID e)
{
    ECS::Entity entity{ e };
    if (entity.HasComponent<Component::RigidBody>()) {
        auto& rb{ entity.GetComponent<Component::RigidBody>() };
        return glm::vec3{ rb.velocity.x, rb.velocity.y, rb.velocity.z };
    }
    return glm::vec3();
}

MonoString* Mono::GetLayerName(ECS::Entity::EntityID e)
{
    ECS::Entity entity{ e };
    if (entity.HasComponent<Component::Layer>()) {
        auto& layer{ entity.GetComponent<Component::Layer>() };
        return STDToMonoString(layer.name);
    }
    return nullptr;
}

float Mono::GetDeltaTime()
{
  return Performance::FrameRateController::GetInstance().GetDeltaTime();
}


MonoObject* Mono::FindScript(MonoString* s)
{
  std::string msg{ MonoStringToSTD(s) };
  for (ECS::Entity e : ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::Script>())
  {
    for ( Mono::ScriptInstance& SI : e.GetComponent<Component::Script>().mScriptList)
      if (SI.mScriptName == msg)
        return SI.mClassInst;
  }
  return nullptr;
}


void Mono::DestroyEntity(ECS::Entity::EntityID entity)
{
  if (ECS::Entity(entity))
    ECS::EntityManager::GetInstance().RemoveEntity(ECS::Entity(entity)); 
  else
    Debug::DebugLogger::GetInstance().LogWarning("Your r trying to delete an entity doesn't exist");
}

void Mono::DestroyScript(MonoObject* obj, ECS::Entity::EntityID entity)
{
  if (ECS::Entity(entity) && ECS::Entity(entity).HasComponent<Component::Script>())
  {
    Component::Script& s = ECS::Entity(entity).GetComponent<Component::Script>();
    int pos{ 0 };
    bool hasScript{ false };
    for (Mono::ScriptInstance& si : s.mScriptList)
    {
      if (si.mClassInst == obj)
      {
        hasScript = true;
        break;
      }
        
      ++pos;
    }
    if (hasScript)
    {
      s.mScriptList[pos].FreeScript();
      s.mScriptList.erase(s.mScriptList.begin() + pos);
    }
      
    else
      Debug::DebugLogger::GetInstance().LogWarning("Your r trying to delete a script that doesn't exist ");
  }
  else
    Debug::DebugLogger::GetInstance().LogWarning("The entity itself doesnt have a script, or the entity doesnt exist");
}


void Mono::SetActive(ECS::Entity::EntityID entity, bool b)
{
  if (ECS::Entity(entity))
  {
    ECS::Entity(entity).SetIsActive(b);
  }
  else
    Debug::DebugLogger::GetInstance().LogError("You r trying to set active on an invalid entity");
}


/*!**********************************************************************
*																																			  *
*								  Helper Functions to get data from C#			           	*
*																																			  *
************************************************************************/

bool Mono::CheckMonoError(MonoError& error)
{
  bool hasError = !mono_error_ok(&error);
  if (hasError)
  {
    unsigned short errorCode = mono_error_get_error_code(&error);
    const char* errorMessage = mono_error_get_message(&error);
    printf("Mono Error!\n");
    printf("\tError Code: %hu\n", errorCode);
    printf("\tError Message: %s\n", errorMessage);
    mono_error_cleanup(&error);
  }
  return hasError;
}

std::string Mono::MonoStringToSTD(MonoString* str)
{
  if (str == nullptr || mono_string_length(str) == 0)
    return"";

  MonoError error;
  char* utf8 = mono_string_to_utf8_checked(str, &error);
  if (CheckMonoError(error))
    return "";

  std::string result(utf8);
  mono_free(utf8);

  return result;

}

MonoString* Mono::STDToMonoString(const std::string& str)
{
  Mono::ScriptManager* sm = &Mono::ScriptManager::GetInstance();
  return (mono_string_new(sm->mAppDomain.get(), str.c_str()));

}