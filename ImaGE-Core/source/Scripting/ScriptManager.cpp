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

#define STBI_MSC_SECURE_CRT
#include "stb/stb_image_write.h"
#include "ScriptManager.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/tabledefs.h"
#include "mono/metadata/mono-debug.h"
#include "mono/metadata/threads.h"
#include <Prefabs/PrefabManager.h>
#include <Core/Systems/TransformSystem/TransformHelpers.h>
#include <Scenes/SceneManager.h>
#include <cstdint>
#include <Scripting/ScriptUtils.h>

#include <Events/EventManager.h>
#include <Scenes/SceneManager.h>
#include <filesystem>
#include <Core/Components/Components.h>
#include <Serialization/Deserializer.h>
#include <Scripting/ScriptInstance.h>
#include "Input/InputManager.h"
#include "Asset/AssetManager.h"
#include "Physics/PhysicsSystem.h"
#include "Graphics/RenderSystem.h"
#include "Graphics/PostProcessing/PostProcessingManager.h"
//#define DEBUG_MONO
namespace Mono
{
  std::map<std::string, ScriptClassInfo> ScriptManager::mMonoClassMap{};
  std::map<std::string, MonoClass*> ScriptManager::mMonoUtilsMap{};
  std::shared_ptr<MonoDomain> ScriptManager::mRootDomain{ nullptr };
  MonoDomain* ScriptManager::mAppDomain{ nullptr };
  std::vector<std::string> ScriptManager::mAllScriptNames;
  std::string ScriptManager::mCoreAssFilePath{};
  std::string ScriptManager::mAppDomFilePath{};
  
  // Hot reload
  std::unique_ptr<filewatch::FileWatch<std::string>> ScriptManager::mCsProjWatcher{};
  std::unique_ptr<filewatch::FileWatch<std::string>> ScriptManager::mFileWatcher{};
  std::vector<std::tuple<std::string, int, int>>ScriptManager::mScreenShotInfo{};
  std::string ScriptManager::mScnfilePath{};
  std::string ScriptManager::mCsprojPath{};
  std::string ScriptManager::mBatfilePath{};
  bool ScriptManager::mAssemblyReloadPending{};
  bool ScriptManager::mCSReloadPending{};
  bool ScriptManager::mRebuildCS{};
  bool ScriptManager::mTriggerStart{};

  std::unordered_map<ScriptFieldType,std::string> ScriptManager::mRevClassMap{};
  std::unordered_map<std::string, ScriptFieldType> ScriptManager::mScriptFieldTypeMap
  {
    { "System.Boolean", ScriptFieldType::BOOL },
    { "System.Int16",   ScriptFieldType::SHORT },
    { "System.Int32",   ScriptFieldType::INT },
    { "System.Single",  ScriptFieldType::FLOAT },
    { "System.Double",  ScriptFieldType::DOUBLE },
    { "System.Int64",   ScriptFieldType::LONG },
    { "System.UInt16",  ScriptFieldType::USHORT },
    { "System.UInt32",  ScriptFieldType::UINT },
    { "System.UInt64",  ScriptFieldType::ULONG },
    { "System.String",  ScriptFieldType::STRING },
    { "System.Numerics.Vector3", ScriptFieldType::VEC3 },
    { "IGE.Utils.Vec3<System.Double>", ScriptFieldType::DVEC3 },
    { "System.Int32[]", ScriptFieldType::INT_ARR },
    { "System.Single[]", ScriptFieldType::FLOAT_ARR },
    { "System.Double[]", ScriptFieldType::DOUBLE_ARR },
    { "System.String[]", ScriptFieldType::STRING_ARR},
    { "Entity[]", ScriptFieldType::ENTITY_ARR },
    { "Entity", ScriptFieldType::ENTITY},
    { "Inside", ScriptFieldType::INSIDE},
    { "Test", ScriptFieldType::TEST},
    { "InsideB", ScriptFieldType::INSIDEB},
    { "PlayerMove", ScriptFieldType::PLAYER_MOVE},
    { "Dialogue", ScriptFieldType::DIALOGUE},
    { "PlayerInteraction", ScriptFieldType::PLAYER_INTERACTION },
    { "Inventory", ScriptFieldType::INVENTORY },
    { "TutorialLevelInventory", ScriptFieldType::TUTORIAL_LEVEL_INVENTORY },
    { "Level2Inventory", ScriptFieldType::LEVEL2_INVENTORY },
    { "Level3Inventory", ScriptFieldType::LEVEL3_INVENTORY },
    { "SpecialDialogue", ScriptFieldType::SPECIAL_DIALOGUE },
    { "KeyDoor", ScriptFieldType::KEY_DOOR },
    { "PictureAlign", ScriptFieldType::PICTURE_ALIGN },
    { "ControlPanel2", ScriptFieldType::CONTROL_PANEL },
    { "Transition", ScriptFieldType::TRANSITION },
    { "PlayerArise",ScriptFieldType::PLAYER_ARISE},
    { "EyeBallFollow", ScriptFieldType::EYEBALL_FOLLOW },
    { "HammerLevel3", ScriptFieldType::HAMMER_L3 },
    { "BlackBorder", ScriptFieldType::BLACK_BORDER },
    { "HexTableOrb", ScriptFieldType::HEX_TABLE_ORB },
    { "Level3Dialogue", ScriptFieldType::LEVEL3_DIALOGUE },
    { "Fragment", ScriptFieldType::FRAGMENT },
    { "BootupText", ScriptFieldType::BOOTUPTEXT },
    { "Lvl4Dialogue", ScriptFieldType::LVL4_DIALOGUE },
    { "SettingsPage", ScriptFieldType::SETTINGS },
    { "PauseMenu", ScriptFieldType::PAUSEMENU },
    { "SettingsButtons", ScriptFieldType::SETTINGSBUTTON },
    { "PauseMenuButtons", ScriptFieldType::PAUSEMENUBUTTON },
     { "NewGameCD", ScriptFieldType::NEWCD }


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
#ifdef _DEBUG
  std::cout << "Finish load assembly file\n";
#endif
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
  mAppDomain = mono_domain_create_appdomain(const_cast<char*>(mAppDomFilePath.c_str()), nullptr);
  mono_domain_set(mAppDomain, true);
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
  ADD_CLASS_INTERNAL_CALL(IsKeyPressed, Input::InputManager::GetInstance());
  ADD_CLASS_INTERNAL_CALL(GetInputString, Input::InputManager::GetInstance());
  ADD_CLASS_INTERNAL_CALL(AnyKeyDown, Input::InputManager::GetInstance());
  ADD_CLASS_INTERNAL_CALL(AnyKeyTriggered, Input::InputManager::GetInstance());
  ADD_INTERNAL_CALL(GetMousePos);
  ADD_INTERNAL_CALL(GetScreenDimension);
  ADD_INTERNAL_CALL(GetMousePosWorld);
  ADD_INTERNAL_CALL(GetMouseDelta);
  ADD_INTERNAL_CALL(GetCameraForward);
  ADD_INTERNAL_CALL(GetCameraRight);


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
  ADD_INTERNAL_CALL(GetWorldRotationEuler);
  ADD_INTERNAL_CALL(GetRotationEuler);
  ADD_INTERNAL_CALL(SetRotationEuler);
  ADD_INTERNAL_CALL(SetScale);
  ADD_INTERNAL_CALL(MoveCharacter);
  ADD_INTERNAL_CALL(SetAngularVelocity);
  ADD_INTERNAL_CALL(SetVelocity);
  ADD_INTERNAL_CALL(GetGravityFactor);
  ADD_INTERNAL_CALL(SetGravityFactor);
  ADD_INTERNAL_CALL(LockRigidBody);
  ADD_INTERNAL_CALL(LockRigidBodyRotation);
  ADD_INTERNAL_CALL(SetDynamicFriction);

  //Debug Functions
  ADD_INTERNAL_CALL(Log);
  ADD_INTERNAL_CALL(LogWarning);
  ADD_INTERNAL_CALL(LogError);
  ADD_INTERNAL_CALL(LogCritical);

  //Entity Functions
  ADD_INTERNAL_CALL(GetAxis);
  ADD_INTERNAL_CALL(GetDeltaTime);
  ADD_INTERNAL_CALL(GetTime);
  ADD_INTERNAL_CALL(GetFPS);
  ADD_INTERNAL_CALL(MoveCharacter);
  ADD_INTERNAL_CALL(GetTag);
  ADD_INTERNAL_CALL(SetTag);
  ADD_INTERNAL_CALL(FindScript);
  ADD_INTERNAL_CALL(FindScriptInEntity);
  ADD_INTERNAL_CALL(GetParentByID);
  ADD_INTERNAL_CALL(DestroyEntity);
  ADD_INTERNAL_CALL(DestroyScript);
  ADD_INTERNAL_CALL(SetActive);
  ADD_INTERNAL_CALL(IsActive);
  ADD_INTERNAL_CALL(FindChildByTag);
  ADD_INTERNAL_CALL(FindParentByTag);
  ADD_INTERNAL_CALL(GetAllChildren);
  ADD_INTERNAL_CALL(UnparentEntity);
  ADD_INTERNAL_CALL(GetMainCameraPosition);
  ADD_INTERNAL_CALL(GetMainCameraDirection);
  ADD_INTERNAL_CALL(GetMainCameraRotation);
  ADD_INTERNAL_CALL(GetTextColor);
  ADD_INTERNAL_CALL(SetTextColor);
  ADD_INTERNAL_CALL(GetTextScale);
  ADD_INTERNAL_CALL(SetTextScale);
  ADD_INTERNAL_CALL(SetBloomIntensity);
  ADD_INTERNAL_CALL(GetBloomIntensity);
  ADD_INTERNAL_CALL(SetLightIntensity);
  ADD_INTERNAL_CALL(GetLightIntensity);
  ADD_INTERNAL_CALL(GetText);
  ADD_INTERNAL_CALL(SetText);
  ADD_INTERNAL_CALL(AppendText);
  ADD_INTERNAL_CALL(SetTextFont);
  ADD_INTERNAL_CALL(GetImageColor);
  ADD_INTERNAL_CALL(SetImageColor);
  ADD_INTERNAL_CALL(GetSprite2DColor);
  ADD_INTERNAL_CALL(SetSprite2DColor);
  ADD_INTERNAL_CALL(SetDaySkyBox);
  ADD_INTERNAL_CALL(UpdatePhysicsToTransform);

  // Utility Functions
  ADD_INTERNAL_CALL(Raycast);
  ADD_INTERNAL_CALL(RaycastFromEntity);
  ADD_INTERNAL_CALL(RaycastFromEntityInfo);
  ADD_INTERNAL_CALL(SetSoundPitch);
  ADD_INTERNAL_CALL(SetSoundVolume);
  ADD_INTERNAL_CALL(SetSoundGlobalVolume);
  ADD_INTERNAL_CALL(GetSoundGlobalVolume);
  ADD_INTERNAL_CALL(EnableSoundPostProcessing);
  ADD_INTERNAL_CALL(DisableSoundPostProcessing);
  ADD_INTERNAL_CALL(PlaySound);
  ADD_INTERNAL_CALL(PauseSound);
  ADD_INTERNAL_CALL(StopSound);
  ADD_INTERNAL_CALL(PlaySoundFromPosition);
  ADD_INTERNAL_CALL(GetSoundPlaybackPosition);
  ADD_INTERNAL_CALL(PlayAnimation);
  ADD_INTERNAL_CALL(IsPlayingAnimation);
  ADD_INTERNAL_CALL(PauseAnimation);
  ADD_INTERNAL_CALL(ResumeAnimation);
  ADD_INTERNAL_CALL(StopAnimationLoop);
  ADD_INTERNAL_CALL(GetLayerName);
  ADD_INTERNAL_CALL(GetCurrentScene);
  ADD_INTERNAL_CALL(SetCurrentScene);
  ADD_INTERNAL_CALL(TakeScreenShot);
  ADD_INTERNAL_CALL(ShowCursor);
  ADD_INTERNAL_CALL(HideCursor);
  ADD_INTERNAL_CALL(OnTriggerEnter);
  ADD_INTERNAL_CALL(OnTriggerExit);
  ADD_INTERNAL_CALL(GetContactPoints);
  ADD_INTERNAL_CALL(GetShortestDistance);
  ADD_INTERNAL_CALL(ChangeToolsPainting);
  ADD_INTERNAL_CALL(SpawnToolBox);
  ADD_INTERNAL_CALL(SpawnOpenDoor);
  ADD_INTERNAL_CALL(SpawnTaraSilhouette);
  ADD_INTERNAL_CALL(SetShaderState);
  ADD_INTERNAL_CALL(PauseGame);
  ADD_INTERNAL_CALL(ResumeGame);
  ADD_INTERNAL_CALL(GetIsPaused);
  ADD_INTERNAL_CALL(SetCanvasTransitionProgress);
  ADD_INTERNAL_CALL(EnableCanvasTransition);
  ADD_INTERNAL_CALL(SetCanvasTransitionType);
  ADD_INTERNAL_CALL(SetBrightness);
  ADD_INTERNAL_CALL(SetBGM);
  ADD_INTERNAL_CALL(GetGammaNorm);
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
    else { //tch: i added this for my own custom class
        mMonoUtilsMap[className] = GetClassInAssembly(coreAssembly, classNameSpace.c_str(), className.c_str());
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
#ifdef _DEBUG
    std::cout << "Current directory: " << currentPath << std::endl;
#endif
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

std::vector<ScriptInstance> ScriptManager::SerialMonoObjectVec(std::vector<MonoObject*> const& vec)
{
  std::vector<ScriptInstance> toSer{};
  toSer.reserve(vec.size());

  for (MonoObject* obj : vec) {
    toSer.emplace_back(obj, false, true);
  }

  return toSer;
}

void ScriptManager::LinkAllScriptDataMember()
{
  for (ECS::Entity e : ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::Script>())
  {
    for (ScriptInstance& si : e.GetComponent<Component::Script>().mScriptList)
    {
      for (rttr::variant& i : si.mScriptFieldInstList)
      {
        if (i.is_type<Mono::DataMemberInstance<ScriptInstance>>())
        {
          Mono::DataMemberInstance<ScriptInstance>& sfi = i.get_value<Mono::DataMemberInstance<ScriptInstance>>();
          if (sfi.mData.mClassInst && ECS::EntityManager::GetInstance().IsValidEntity(sfi.mData.mEntityID) && ECS::Entity(sfi.mData.mEntityID).HasComponent<Component::Script>())
          {
            for (ScriptInstance& sij : ECS::Entity(sfi.mData.mEntityID).GetComponent<Component::Script>().mScriptList)
            {
              if (sij.mScriptName == sfi.mData.mScriptName && sij.mClassInst)
              {
                sfi.mData = sij;
                si.SetFieldValue<MonoObject>(sfi.mData.mClassInst, sfi.mScriptField.mClassField);
                break;
              }
              
            }
          }
        }
      }
    }
  }
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
#ifdef _DEBUG
  std::cout << " mono_domain_set(mono_get_root_domain(), false); Done\n";
#endif
//  if(mAppDomain)
//    mAppDomain.reset();
//
//#ifdef _DEBUG
//  std::cout << " mAppDomain.reset(); Done\n";
//#endif
  mMonoClassMap.clear();

#ifdef _DEBUG
  std::cout << "  mMonoClassMap.clear(); Done\n";
#endif
  mAllScriptNames.clear();
#ifdef _DEBUG
  std::cout << " mAllScriptNames.clear(); Done\n";
#endif
  LoadAppDomain();
  //Assets::AssetManager& assetManager{ Assets::AssetManager::GetInstance() };
  mFileWatcher = std::make_unique < filewatch::FileWatch < std::string>>("../Assets/Scripts/ImaGE-Script.dll", AssemblyFileSystemEvent);
  mAssemblyReloadPending = false;
#ifdef _DEBUG
  std::cout << "loadAp domain Reload Done\n";
#endif
  ReloadScripts();

#ifdef _DEBUG
  std::cout << "ASS Reload Done\n";
#endif
}

void ScriptManager::ReloadScripts()
{
  AddInternalCalls();
#ifdef _DEBUG
  std::cout << "Addinternal call Done\n";
#endif
  LoadAllMonoClass();
#ifdef _DEBUG
  std::cout << "load mono class Done\n";
#endif
  ReloadAllScripts();
#ifdef _DEBUG
  std::cout << "load all scripts Done\n";
#endif
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

 
    mono_domain_unload(mAppDomain);

  
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

    MonoObject* classInstance = mono_object_new(mAppDomain, currClass);  //Get a reference to the class we want to instantiate


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
    MonoObject* exception{ nullptr };
    mono_runtime_invoke(classCtor, classInstance, arg.data(), &exception);

    // don't print in game build
#ifndef DISTRIBUTION
    if (exception) {
      mono_print_unhandled_exception(exception);
      IGE_DBGLOGGER.LogError("C# exception thrown from " + std::string(className) + " constructor. Check console for details.");
    }
#endif


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
  TransformHelpers::UpdateLocalTransform(entity);
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
  TransformHelpers::UpdateLocalTransform(entity);
}

glm::vec3 Mono::GetScale(ECS::Entity::EntityID entity)
{
  Component::Transform& trans{ ECS::Entity(entity).GetComponent<Component::Transform>() };
  return trans.scale;
}

void Mono::SetScale(ECS::Entity::EntityID entity, glm::vec3 scale) {
  Component::Transform& trans{ ECS::Entity(entity).GetComponent<Component::Transform>() };
  trans.scale = scale;
  TransformHelpers::UpdateWorldTransform(entity);
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
}

void Mono::SetWorldRotation(ECS::Entity::EntityID entity, glm::quat rotAdjustment)
{
  Component::Transform& trans{ ECS::Entity(entity).GetComponent<Component::Transform>() };
  trans.worldRot = rotAdjustment;
  TransformHelpers::UpdateLocalTransform(entity);
}

glm::quat Mono::GetRotation(ECS::Entity::EntityID entity)
{
  Component::Transform& trans{ ECS::Entity(entity).GetComponent<Component::Transform>() };
  return trans.rotation;
}

void Mono::SetRotation(ECS::Entity::EntityID entity, glm::quat rotAdjustment)
{
  Component::Transform& trans{ ECS::Entity(entity).GetComponent<Component::Transform>() };
  trans.rotation = rotAdjustment;
  trans.eulerAngles = glm::degrees(glm::eulerAngles(rotAdjustment));
  TransformHelpers::UpdateWorldTransform(entity);
}

glm::vec3 Mono::GetWorldRotationEuler(ECS::Entity::EntityID entity)
{
  Component::Transform& trans{ ECS::Entity(entity).GetComponent<Component::Transform>() };
  return trans.GetWorldEulerAngles();
}
glm::vec3 Mono::GetRotationEuler(ECS::Entity::EntityID entity)
{
  Component::Transform& trans{ ECS::Entity(entity).GetComponent<Component::Transform>() };
  return trans.eulerAngles;
}
void Mono::SetRotationEuler(ECS::Entity::EntityID entity, glm::vec3 rotAdjustment)
{
  Component::Transform& trans{ ECS::Entity(entity).GetComponent<Component::Transform>() };
  trans.SetLocalRotWithEuler(rotAdjustment);
  TransformHelpers::UpdateWorldTransform(entity);
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
  if (ECS::Entity(entity).HasComponent<Component::Tag>())
    return STDToMonoString(ECS::Entity(entity).GetComponent<Component::Tag>().tag);
  else
    return STDToMonoString("");

  // @TODO: TEMP
  //return STDToMonoString(ECS::Entity(entity).GetComponent<Component::Tag>().tag);
}

void Mono::SetTag(ECS::Entity::EntityID entity, MonoString* tag) {
  std::string convertedTag{ MonoStringToSTD(tag) };
  if (ECS::Entity(entity).HasComponent<Component::Tag>())
    ECS::Entity(entity).GetComponent<Component::Tag>().tag = convertedTag;
}



void Mono::Log(MonoString*s)
{
  std::string msg{ MonoStringToSTD(s) };
  Debug::DebugLogger::GetInstance().LogInfo(msg);
}

void Mono::LogWarning(MonoString* s)
{
  std::string msg{ MonoStringToSTD(s) };
  Debug::DebugLogger::GetInstance().LogWarning(msg);
}

void Mono::LogError(MonoString* s)
{
  std::string msg{ MonoStringToSTD(s) };
  Debug::DebugLogger::GetInstance().LogError(msg);
}

void Mono::LogCritical(MonoString* s)
{
  std::string msg{ MonoStringToSTD(s) };
  Debug::DebugLogger::GetInstance().LogCritical(msg);
}

float Mono::GetAxis(MonoString* s)
{
  std::string msg{ MonoStringToSTD(s) };
  return Input::InputManager::GetInstance().GetAxis(msg);
}

void Mono::MoveCharacter(ECS::Entity::EntityID entity, glm::vec3 dVec) {
  if (!ECS::Entity(entity).HasComponent<Component::RigidBody>()) {
    Debug::DebugLogger::GetInstance().LogError("Entity does not have the RigidBody component");
    return;
  }
  
  if (glm::length(dVec) < FLT_EPSILON) { // dont move anything if velocity is 0
    return;
  }
  Performance::FrameRateController::TimeType dt = Performance::FrameRateController::GetInstance().GetDeltaTime();
  ECS::Entity(entity).GetComponent<Component::RigidBody>().force.x = dVec.x;
  //ECS::Entity(entity).GetComponent<Component::RigidBody>().velocity.y = dVec.y * dt;
  ECS::Entity(entity).GetComponent<Component::RigidBody>().force.z = dVec.z;
     
  IGE::Physics::PhysicsSystem::GetInstance().get()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::FORCE);
}

void Mono::SetAngularVelocity(ECS::Entity::EntityID entity, glm::vec3 angularVelocity) {
  if (!ECS::Entity(entity).HasComponent<Component::RigidBody>()) {
    Debug::DebugLogger::GetInstance().LogError("Entity does not have the RigidBody component");
    return;
  }

  Performance::FrameRateController::TimeType dt = Performance::FrameRateController::GetInstance().GetDeltaTime();
  ECS::Entity(entity).GetComponent<Component::RigidBody>().angularVelocity.x = angularVelocity.x * dt;
  ECS::Entity(entity).GetComponent<Component::RigidBody>().angularVelocity.y = angularVelocity.y * dt;
  ECS::Entity(entity).GetComponent<Component::RigidBody>().angularVelocity.z = angularVelocity.z * dt;

  IGE::Physics::PhysicsSystem::GetInstance().get()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::ANGULAR_VELOCITY);
}

void Mono::SetVelocity(ECS::Entity::EntityID entity, glm::vec3 velocity) {
  if (!ECS::Entity(entity).HasComponent<Component::RigidBody>()) {
    Debug::DebugLogger::GetInstance().LogError("Entity does not have the RigidBody component");
    return;
  }

  Performance::FrameRateController::TimeType dt = Performance::FrameRateController::GetInstance().GetDeltaTime();
  ECS::Entity(entity).GetComponent<Component::RigidBody>().velocity.x = velocity.x * dt;
  ECS::Entity(entity).GetComponent<Component::RigidBody>().velocity.y = velocity.y * dt;
  ECS::Entity(entity).GetComponent<Component::RigidBody>().velocity.z = velocity.z * dt;

  IGE::Physics::PhysicsSystem::GetInstance().get()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::VELOCITY);
}

float Mono::GetGravityFactor(ECS::Entity::EntityID entity) {
  if (!ECS::Entity(entity).HasComponent<Component::RigidBody>()) {
    Debug::DebugLogger::GetInstance().LogError("Entity does not have the RigidBody component");
    return 0.f;
  }

  return ECS::Entity(entity).GetComponent<Component::RigidBody>().gravityFactor;
}

void Mono::SetGravityFactor(ECS::Entity::EntityID entityId, float gravity) {
  ECS::Entity entity{ entityId };
  if (!entity.HasComponent<Component::RigidBody>()) {
    Debug::DebugLogger::GetInstance().LogError("Entity " + entity.GetTag() + " does not have the RigidBody component");
    return;
  }
  entity.GetComponent<Component::RigidBody>().gravityFactor = gravity;
  IGE::Physics::PhysicsSystem::GetInstance().get()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::GRAVITY_FACTOR);
  //IGE_DBGLOGGER.LogInfo("Set gravity of " + entity.GetTag() + " to " + std::to_string(gravity));
}

void Mono::LockRigidBody(ECS::Entity::EntityID entityId, bool lock) {
  ECS::Entity entity{ entityId };
  if (!entity.HasComponent<Component::RigidBody>()) {
    Debug::DebugLogger::GetInstance().LogError("Entity " + entity.GetTag() + " does not have the RigidBody component");
    return;
  }

  Component::RigidBody& rigidBody{ entity.GetComponent<Component::RigidBody>() };
  if (lock) {
    int const combinedAxes{ 
      (int)Component::RigidBody::Axis::X | (int)Component::RigidBody::Axis::Y | (int)Component::RigidBody::Axis::Z
    };
    rigidBody.SetAxisLock(combinedAxes);
    rigidBody.SetAngleAxisLock(combinedAxes);
  }
  else {
    rigidBody.axisLock = 0;
    rigidBody.angularAxisLock = 0;
  }
  IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::LOCK);
}

void Mono::LockRigidBodyRotation(ECS::Entity::EntityID entityId, bool x, bool y, bool z) {
  ECS::Entity entity{ entityId };
  if (!entity.HasComponent<Component::RigidBody>()) {
    Debug::DebugLogger::GetInstance().LogError("Entity " + entity.GetTag() + " does not have the RigidBody component");
    return;
  }

  Component::RigidBody& rigidBody{ entity.GetComponent<Component::RigidBody>() };
  rigidBody.angularAxisLock = 0;
  rigidBody.SetAngleAxisLock(
    (x ? (int)Component::RigidBody::Axis::X : 0) |
    (y ? (int)Component::RigidBody::Axis::Y : 0) |
    (z ? (int)Component::RigidBody::Axis::Z : 0)
  );
  IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::LOCK);
}

void Mono::SetDynamicFriction(ECS::Entity::EntityID entityId, float val) {
  ECS::Entity entity{ entityId };
  entity.GetComponent<Component::RigidBody>().dynamicFriction = val;
  IGE::Physics::PhysicsSystem::GetInstance()->ChangeRigidBodyVar(entity, Component::RigidBodyVars::DYNAMIC_FRICTION);
  //IGE_DBGLOGGER.LogInfo("Set dynamic friction of " + entity.GetTag() + " to " + std::to_string(val));
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

glm::vec3 Mono::GetScreenDimension()
{
  return glm::vec3(Input::InputManager::GetInstance().GetDim(),1.f);
}

void Mono::SetBrightness(float fraction)
{
  float newValue = Component::Light::sGlobalProps.MinGammaValue + (fraction * (Component::Light::sGlobalProps.MaxGammvalue - Component::Light::sGlobalProps.MinGammaValue));
  Component::Light::sGlobalProps.gammaValue = newValue;
}

void Mono::SetBGM(float fraction)
{

}

float Mono::GetGammaNorm()
{
  return (Component::Light::sGlobalProps.gammaValue - Component::Light::sGlobalProps.MinGammaValue) / (Component::Light::sGlobalProps.MaxGammvalue - Component::Light::sGlobalProps.MinGammaValue);
}

glm::vec3 Mono::GetMousePosWorld(float depth)
{
    return Input::InputManager::GetInstance().GetMousePosWorld(depth);
}

glm::vec3 Mono::GetCameraForward()
{
    if (Graphics::RenderSystem::mCameraManager.HasActiveCamera()) {
        return Graphics::RenderSystem::mCameraManager.GetActiveCameraComponent().GetForwardVector();
    }
    return { 1,0,0 };
}

glm::vec3 Mono::GetCameraRight()
{
  if (Graphics::RenderSystem::mCameraManager.HasActiveCamera()) {
    return Graphics::RenderSystem::mCameraManager.GetActiveCameraComponent().GetRightVector();
  }
  return { 1,0,0 };
}

ECS::Entity::EntityID Mono::Raycast(glm::vec3 start, glm::vec3 end)
{
    IGE::Physics::RaycastHit hit{};
    if (IGE::Physics::PhysicsSystem::GetInstance()->RayCastSingular(start, end, hit)) {
        ECS::Entity::EntityID out {hit.entity.GetEntityID()};
        return out;
    }return static_cast<ECS::Entity::EntityID>(static_cast<unsigned>(-1));
}

ECS::Entity::EntityID Mono::RaycastFromEntity(ECS::Entity::EntityID e, glm::vec3 start, glm::vec3 end) {
    IGE::Physics::RaycastHit hit{};
    if (IGE::Physics::PhysicsSystem::GetInstance()->RayCastFromEntity(e, start, end, hit)) {
        ECS::Entity::EntityID out {hit.entity.GetEntityID()};
        return out;
    }return static_cast<ECS::Entity::EntityID>(static_cast<unsigned>(-1));
}

IGE::Physics::RaycastHitInfo Mono::RaycastFromEntityInfo(ECS::Entity::EntityID e, glm::vec3 start, glm::vec3 end) {
    IGE::Physics::RaycastHit hit{};
    if (IGE::Physics::PhysicsSystem::GetInstance()->RayCastFromEntity(e, start, end, hit)) {
        ECS::Entity::EntityID out{ hit.entity.GetEntityID() };

        return IGE::Physics::RaycastHitInfo{ hit.position, hit.normal, hit.distance };
    }return IGE::Physics::RaycastHitInfo{};
}

void Mono::SetSoundPitch(ECS::Entity::EntityID e, MonoString* s, float p)
{
    std::string const name{ MonoStringToSTD(s) };
    ECS::Entity entity{ e };
    entity.GetComponent<Component::AudioSource>().SetSoundPitch(name, p);
}

void Mono::SetSoundVolume(ECS::Entity::EntityID e, MonoString* s, float v) 
{
    std::string const name{ MonoStringToSTD(s) };
    ECS::Entity entity{ e };
    //printf("%s is setting volume", entity.GetComponent<Component::Tag>().tag.c_str());
    entity.GetComponent<Component::AudioSource>().SetSoundVolume(name, v);
}

void Mono::SetSoundGlobalVolume(float vol) {
    IGE::Audio::AudioManager::GetInstance().mGlobalVolume = vol;
}


float Mono::GetSoundGlobalVolume() {
  return IGE::Audio::AudioManager::GetInstance().mGlobalVolume;
}

void Mono::EnableSoundPostProcessing(ECS::Entity::EntityID e, MonoString* s, unsigned type, float param)
{
    std::string const name{ MonoStringToSTD(s) };
    ECS::Entity entity{ e };
    entity.GetComponent<Component::AudioSource>().EnablePostProcessing(name, static_cast<IGE::Audio::PostProcessingType>(type), param);
}

void Mono::DisableSoundPostProcessing(ECS::Entity::EntityID e, MonoString* s)
{
    std::string const name{ MonoStringToSTD(s) };
    ECS::Entity entity{ e };
    entity.GetComponent<Component::AudioSource>().DisablePostProcessing(name);
}

void Mono::PlaySound(ECS::Entity::EntityID e, MonoString* s)
{
    std::string const name{ MonoStringToSTD(s) };
    ECS::Entity entity{ e };
    entity.GetComponent<Component::AudioSource>().PlaySound(name);
}

void Mono::PauseSound(ECS::Entity::EntityID e, MonoString* s)
{
    std::string const name{ MonoStringToSTD(s) };
    ECS::Entity entity{ e };
    entity.GetComponent<Component::AudioSource>().PauseSound(name);
}

void Mono::PlaySoundFromPosition(ECS::Entity::EntityID e, MonoString* s, unsigned time)
{
    std::string const name{ MonoStringToSTD(s) };
    ECS::Entity entity{ e };
    entity.GetComponent<Component::AudioSource>().SetPlaybackTime(name, time);
}

unsigned Mono::GetSoundPlaybackPosition(ECS::Entity::EntityID e, MonoString* s)
{
    std::string const name{ MonoStringToSTD(s) };
    ECS::Entity entity{ e };
    return entity.GetComponent<Component::AudioSource>().GetPlaybackTime(name);
}

void Mono::StopSound(ECS::Entity::EntityID e, MonoString* s)
{
    std::string const name{ MonoStringToSTD(s) };
    ECS::Entity entity{ e };
    entity.GetComponent<Component::AudioSource>().StopSound(name);
}

void Mono::PlayAnimation(ECS::Entity::EntityID entity, MonoString* str, bool loop) {
  std::string const name{ MonoStringToSTD(str) };
  ECS::Entity(entity).GetComponent<Component::Animation>().PlayAnimation(name, loop);
}
bool Mono::IsPlayingAnimation(ECS::Entity::EntityID entityId) {
  ECS::Entity const entity{ entityId };
  if (!entity.HasComponent<Component::Animation>()) {
    IGE_DBGLOGGER.LogError("IsPlayingAnimation(): Entity " + entity.GetTag() + " does not have an animation component!");
    return false;
  }

  return entity.GetComponent<Component::Animation>().IsPlayingAnimation();
}

MonoString* GetCurrentAnimation(ECS::Entity::EntityID entityId) {
  ECS::Entity const entity{ entityId };
  if (!entity.HasComponent<Component::Animation>()) {
    IGE_DBGLOGGER.LogError("IsPlayingAnimation(): Entity " + entity.GetTag() + " does not have an animation component!");
    return nullptr;
  }

  return STDToMonoString(entity.GetComponent<Component::Animation>().currentAnimation.first);
}

void Mono::PauseAnimation(ECS::Entity::EntityID entity) {
  ECS::Entity(entity).GetComponent<Component::Animation>().Pause();
}
void Mono::ResumeAnimation(ECS::Entity::EntityID entity) {
  ECS::Entity(entity).GetComponent<Component::Animation>().Resume();
}

void Mono::StopAnimationLoop(ECS::Entity::EntityID entity) {
  ECS::Entity(entity).GetComponent<Component::Animation>().repeat = false;
}

void Mono::SetShaderState(unsigned idx, bool active)
{
    Graphics::PostProcessingManager::GetInstance().SetShaderState(idx, active);
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

void Mono::UpdatePhysicsToTransform(ECS::Entity::EntityID entity) {
  IGE::Physics::PhysicsSystem::GetInstance()->UpdatePhysicsToTransform(entity);
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

float Mono::GetTime() {
  return Performance::FrameRateController::GetInstance().GetTime();
}

float Mono::GetFPS() {
  return Performance::FrameRateController::GetInstance().GetFPS();
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


MonoObject* Mono::FindScriptInEntity(ECS::Entity::EntityID entity, MonoString* s)
{
  std::string msg{ MonoStringToSTD(s) };
  if(ECS::Entity(entity).HasComponent<Component::Script>())
    for (Mono::ScriptInstance& SI : ECS::Entity(entity).GetComponent<Component::Script>().mScriptList)
      if (SI.mScriptName == msg)
        return SI.mClassInst;
  
  return nullptr;
}

ECS::Entity::EntityID Mono::FindChildByTag(ECS::Entity::EntityID entity, MonoString* s)
{
  std::string msg{ MonoStringToSTD(s) };
  if (ECS::Entity(entity) && ECS::EntityManager::GetInstance().HasChild(entity))
  {
    for (ECS::Entity e : ECS::EntityManager::GetInstance().GetChildEntity(ECS::Entity(entity)))
     {
        if (e.GetTag() == msg)
          return e.GetRawEnttEntityID();
    }
  }
  Debug::DebugLogger::GetInstance().LogError("You are trying to find a child entity with an invalid name");
  return static_cast<ECS::Entity::EntityID>(std::numeric_limits<std::uint32_t>::max());
}


ECS::Entity::EntityID Mono::FindParentByTag(MonoString* s) {
  std::string msg{ MonoStringToSTD(s) };
  for (ECS::Entity e : ECS::EntityManager::GetInstance().GetAllEntities()) {
      if (e.GetTag() == msg) {
          return e.GetRawEnttEntityID();
      }
  }
  return ECS::Entity::EntityID();
}


ECS::Entity::EntityID Mono::GetParentByID(ECS::Entity::EntityID entity)
{
  if (ECS::Entity(entity) && ECS::EntityManager::GetInstance().HasParent(entity))
  {
    return ECS::EntityManager::GetInstance().GetParentEntity(entity).GetRawEnttEntityID();
  }
  Debug::DebugLogger::GetInstance().LogError("You are trying to find the parent of an entity that doesnt have one");
  return static_cast<ECS::Entity::EntityID>(std::numeric_limits<std::uint32_t>::max());
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

    if (ECS::EntityManager::GetInstance().HasChild(entity)) {
      for (ECS::Entity e : ECS::EntityManager::GetInstance().GetChildEntity(ECS::Entity(entity))) {
        SetActive(e.GetRawEnttEntityID(), b);
      }
    }
  }
  else
    Debug::DebugLogger::GetInstance().LogError("You r trying to set active on an invalid entity");
}

bool Mono::IsActive(ECS::Entity::EntityID entity) {
  if (ECS::Entity{ entity } && ECS::Entity{ entity }.HasComponent<Component::Tag>()) {
    return ECS::Entity{ entity }.GetComponent<Component::Tag>().isActive;
  }
  else {
    Debug::DebugLogger::GetInstance().LogError("Entity does not have the tag component!");
    return false;
  }
}

MonoArray* Mono::GetAllChildren(ECS::Entity::EntityID entity)
{
  std::vector<ECS::Entity::EntityID> allChildren{};
  if (ECS::Entity(entity))
  {
    for (ECS::Entity e : ECS::EntityManager::GetInstance().GetChildEntity(ECS::Entity(entity)))
    {
      allChildren.push_back(e.GetRawEnttEntityID());
    }
  }
  else
    Debug::DebugLogger::GetInstance().LogError("You r trying to set active on an invalid entity");


  MonoArray* newArray = Mono::GetMonoArray<unsigned>(ScriptManager::GetInstance().mAppDomain, allChildren.size());
  for (int i = 0; i < mono_array_length(newArray); ++i) {
    mono_array_set(newArray, ECS::Entity::EntityID, i, allChildren[i]);
  }
  return newArray;
}

void Mono::UnparentEntity(ECS::Entity::EntityID entityId) {
  ECS::Entity entity{ entityId };
  IGE_ENTITYMGR.RemoveParent(entity);
  TransformHelpers::UpdateLocalTransform(entity);
}

glm::vec3 Mono::GetMainCameraPosition(ECS::Entity::EntityID cameraEntity) {
  if (ECS::Entity(cameraEntity) && ECS::Entity{ cameraEntity }.HasComponent<Component::Camera>()) {
    return ECS::Entity{ cameraEntity }.GetComponent<Component::Camera>().position;
  }

  else {
    Debug::DebugLogger::GetInstance().LogError("You are trying to get the camera position of a non-camera Entity!");
  }
  return glm::vec3();
}

glm::vec3 Mono::GetMainCameraDirection(ECS::Entity::EntityID cameraEntity) {
  if (ECS::Entity(cameraEntity) && ECS::Entity{ cameraEntity }.HasComponent<Component::Camera>()) {
    return glm::normalize(ECS::Entity{ cameraEntity }.GetComponent<Component::Camera>().rotation * glm::vec3(0.0f, 0.0f, -1.0f));
  }

  else {
    Debug::DebugLogger::GetInstance().LogError("You are trying to get the camera position of a non-camera Entity!");
  }
  return glm::vec3();
}

glm::quat Mono::GetMainCameraRotation(ECS::Entity::EntityID cameraEntity) {
  if (ECS::Entity(cameraEntity) && ECS::Entity{ cameraEntity }.HasComponent<Component::Camera>()) {
    const auto rot = glm::eulerAngles(ECS::Entity(cameraEntity).GetComponent<Component::Transform>().worldRot);
#ifdef _DEBUG
    std::cout << "CPP: " << rot.x << "," << rot.y  << "," << rot.z << "\n";
#endif
    return ECS::Entity{ cameraEntity }.GetComponent<Component::Transform>().worldRot;
  }
  else {
    Debug::DebugLogger::GetInstance().LogError("You are trying to get the camera position of a non-camera Entity!");
  }
  return glm::quat();
}

glm::vec4 Mono::GetTextColor(ECS::Entity::EntityID textEntity) {
  if (!ECS::Entity{ textEntity }.HasComponent<Component::Text>()) {
    Debug::DebugLogger::GetInstance().LogError("You are trying to Get Text Color of an entity that does not have the Text Component");
    return glm::vec4();
  }

  return ECS::Entity{ textEntity }.GetComponent<Component::Text>().color;
}

void Mono::SetTextColor(ECS::Entity::EntityID textEntity, glm::vec4 textColor) {
  if (!ECS::Entity{ textEntity }.HasComponent<Component::Text>()) {
    Debug::DebugLogger::GetInstance().LogError("You are trying to Set Text Color of an entity that does not have the Text Component");
    return;
  }

  ECS::Entity{ textEntity }.GetComponent<Component::Text>().color = textColor;
}

float Mono::GetTextScale(ECS::Entity::EntityID textEntity) {
  if (!ECS::Entity{ textEntity }.HasComponent<Component::Text>()) {
    Debug::DebugLogger::GetInstance().LogError("You are trying to Get Text Scale of an entity that does not have the Text Component");
    return 0;
  }

  return ECS::Entity{ textEntity }.GetComponent<Component::Text>().scale;
}

void Mono::SetTextScale(ECS::Entity::EntityID textEntity, float textScale) {
  if (!ECS::Entity{ textEntity }.HasComponent<Component::Text>()) {
    Debug::DebugLogger::GetInstance().LogError("You are trying to Get Text Scale of an entity that does not have the Text Component");
    return;
  }

  ECS::Entity{ textEntity }.GetComponent<Component::Text>().scale = textScale;
}

MonoString* Mono::GetText(ECS::Entity::EntityID entity)
{
  std::string msg{};
  if (ECS::Entity(entity))
  {
    if (ECS::Entity(entity).HasComponent<Component::Text>())
      msg = ECS::Entity(entity).GetComponent<Component::Text>().textContent;
    else
      Debug::DebugLogger::GetInstance().LogError("You are trying to Get text from an entity that does not have text component");
  }
  else
    Debug::DebugLogger::GetInstance().LogError("You are trying to Get text from an invalid entity");

  return STDToMonoString(msg);
}

void Mono::SetText(ECS::Entity::EntityID textEntity, MonoString* textContent) {
  std::string scriptTextContent{ MonoStringToSTD(textContent) };
  
  if (!ECS::Entity{ textEntity }.HasComponent<Component::Text>()) {
    Debug::DebugLogger::GetInstance().LogError("You are trying to Set Text of an entity that does not have the Text Component");
    return;
  }

  Component::Text& TextComponent{ ECS::Entity{ textEntity }.GetComponent<Component::Text>() };
  TextComponent.textContent = scriptTextContent;
  TextComponent.newLineIndicesUpdatedFlag = false;
}

void Mono::AppendText(ECS::Entity::EntityID textEntity, MonoString* textContent) {
  std::string scriptTextContent{ MonoStringToSTD(textContent) };

  if (!ECS::Entity{ textEntity }.HasComponent<Component::Text>()) {
    Debug::DebugLogger::GetInstance().LogError("You are trying to Append Text of an entity that does not have the Text Component");
    return;
  }

  Component::Text& TextComponent{ ECS::Entity{ textEntity }.GetComponent<Component::Text>() };
  TextComponent.textContent += scriptTextContent;
  TextComponent.newLineIndicesUpdatedFlag = false;
}

void Mono::SetTextFont(ECS::Entity::EntityID textEntity, MonoString* s) {
  std::string scriptTextFont{ MonoStringToSTD(s) };

  if (!ECS::Entity{ textEntity }.HasComponent<Component::Text>()) {
    Debug::DebugLogger::GetInstance().LogError("You are trying to set the font of an entity that does not have the Text Component");
    return;
  }

  ECS::Entity{ textEntity }.GetComponent<Component::Text>().textAsset = IGE_ASSETMGR.LoadRef<IGE::Assets::FontAsset>(scriptTextFont);
  ECS::Entity{ textEntity }.GetComponent<Component::Text>().fontFamilyName = scriptTextFont;
  ECS::Entity{ textEntity }.GetComponent<Component::Text>().newLineIndicesUpdatedFlag = false;
}


glm::vec4 Mono::GetImageColor(ECS::Entity::EntityID entity)
{
  if (ECS::Entity(entity)) {
    if (ECS::Entity(entity).HasComponent<Component::Image>())
      return ECS::Entity(entity).GetComponent<Component::Image>().color;
    else
      Debug::DebugLogger::GetInstance().LogError("GetImageColor: Entity " + ECS::Entity(entity).GetTag() + " does not have an Image component");
  }
  else {
    Debug::DebugLogger::GetInstance().LogError("GetImageColor: No entity with ID: " + std::to_string(static_cast<uint32_t>(entity)));
  }

  return {};
}

void Mono::SetImageColor(ECS::Entity::EntityID entity, glm::vec4 val)
{
  if (ECS::Entity(entity))
  {
    if (ECS::Entity(entity).HasComponent<Component::Image>())
       ECS::Entity(entity).GetComponent<Component::Image>().color = val;
    else
      Debug::DebugLogger::GetInstance().LogError("SetImageColor: Entity " + ECS::Entity(entity).GetTag() + " does not have an Image component");
  }
  else {
    Debug::DebugLogger::GetInstance().LogError("SetImageColor: No entity with ID: " + std::to_string(static_cast<uint32_t>(entity)));
  }
}

glm::vec4 Mono::GetSprite2DColor(ECS::Entity::EntityID entity)
{
  if (ECS::Entity(entity)) {
    if (ECS::Entity(entity).HasComponent<Component::Sprite2D>())
      return ECS::Entity(entity).GetComponent<Component::Sprite2D>().color;
    else
      Debug::DebugLogger::GetInstance().LogError("GetSprite2DColor: Entity " + ECS::Entity(entity).GetTag() + " does not have an Sprite2D component");
  }
  else {
    Debug::DebugLogger::GetInstance().LogError("GetSprite2DColor: No entity with ID: " + std::to_string(static_cast<uint32_t>(entity)));
  }

  return {};
}

void Mono::SetSprite2DColor(ECS::Entity::EntityID entity, glm::vec4 val)
{
  if (ECS::Entity(entity))
  {
    if (ECS::Entity(entity).HasComponent<Component::Sprite2D>())
      ECS::Entity(entity).GetComponent<Component::Sprite2D>().color = val;
    else
      Debug::DebugLogger::GetInstance().LogError("SetSprite2DColor: Entity " + ECS::Entity(entity).GetTag() + " does not have an Sprite2D component");
  }
  else {
    Debug::DebugLogger::GetInstance().LogError("SetSprite2DColor: No entity with ID: " + std::to_string(static_cast<uint32_t>(entity)));
  }
}

MonoString* Mono::GetCurrentScene() {
  return STDToMonoString(IGE_SCENEMGR.GetSceneName());
}

void Mono::SetCurrentScene(MonoString* scenePath) {
  std::string scenePathSTD{ MonoStringToSTD(scenePath) };

  if (!scenePathSTD.empty()) {
    QUEUE_EVENT(Events::LoadSceneEvent, std::filesystem::path(scenePathSTD).stem().string(),
      scenePathSTD);
  }
}

void Mono::TakeScreenShot(MonoString* name, int width, int height)
{
  Mono::ScriptManager::GetInstance().mScreenShotInfo.push_back({ MonoStringToSTD(name) ,width,height });
}

void Mono::SaveScreenShot(std::string name, int width, int height)
{
  // Get the primary monitor and its video mode
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  if (!monitor)
  {
#ifdef _DEBUG
    std::cout << " Cant find Monitor\n";
#endif
    return;
  }
   
  const GLFWvidmode* mode = glfwGetVideoMode(monitor);
  if (!mode)
  {
#ifdef _DEBUG
    std::cout << "Can't find mode\n";
#endif
    return;
  }
  

  // Window's full width and height
  int windowWidth = mode->width;
  int windowHeight = mode->height;
#ifdef _DEBUG
  std::cout << "Width: " << windowWidth << "  Height:" << windowHeight << "\n";
#endif

  // Calculate the starting coordinates for capturing the center
  int startX = (windowWidth - width) / 2;
  int startY = (windowHeight - height) / 2;

  // Ensure the capture area is within bounds
  if (startX < 0 || startY < 0 || startX + width > windowWidth || startY + height > windowHeight) {
#ifdef _DEBUG
    std::cerr << "Invalid dimensions for screenshot. Check width and height." << std::endl;
#endif
    return;
  }

  // Allocate memory for pixel data
  std::vector<unsigned char> pixels(width * height * 3);

  // Read pixels from the specified region
  glReadPixels(startX, startY, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

  // Flip rows (OpenGL's origin is bo ttom-left, PNG expects top-left)
  std::vector<unsigned char> flippedPixels(width * height * 3);
  for (int y = 0; y < height; ++y)
  {
    memcpy(&flippedPixels[y * width * 3], &pixels[(height - 1 - y) * width * 3], width * 3);
  }

  // Save as PNG
  if (stbi_write_png(("../Assets/GameImg/" + name + ".png").c_str(), width, height, 3, flippedPixels.data(), width * 3))
  {
#ifdef _DEBUG
    std::cout << "Screenshot saved to " << (name + ".png") << std::endl;
#endif
  }
  else
  {
#ifdef _DEBUG
    std::cerr << "Failed to save screenshot!" << std::endl;
#endif
  }
}

bool Mono::SetDaySkyBox(ECS::Entity::EntityID cameraEntity, float speed) {
  //
  ECS::Entity e = ECS::EntityManager::GetInstance().GetEntityFromTag("[Folder] Lights");
  if (ECS::Entity(e))
  {
    for (ECS::Entity child : ECS::EntityManager::GetInstance().GetChildEntity(e))
    {
      if (child.GetTag() == "Props_CeilingLight")
      {
        for (ECS::Entity gchild : ECS::EntityManager::GetInstance().GetChildEntity(child))
        {
          if (gchild.GetTag() == "Light")
          {
            Component::Light& l = gchild.GetComponent<Component::Light>();
            l.mRange = 21.f;
            l.color.r += (0.67f - l.color.r) * Performance::FrameRateController::GetInstance().GetDeltaTime() * speed;
            l.color.g += (1.f - l.color.g) * Performance::FrameRateController::GetInstance().GetDeltaTime() * speed;
            l.color.b += (0.96f - l.color.b) * Performance::FrameRateController::GetInstance().GetDeltaTime() * speed;
            if (l.color.r > 0.67 || l.color.g > 1.f || l.color.b > 0.96f)
            {
              l.color.r = 0.67f;
              l.color.g = 1.f;
              l.color.b = 0.96f;
            }
            std::cout << l.color.r << "\n";
          }

          if (gchild.GetTag() == "CeilingLightBloom") {
            gchild.SetIsActive(true);
          }
        }
      }
      else if(child.GetTag() == "Tools Spotlight")
      {
        child.SetIsActive(true);
      }
      else
      {
        if (child.HasComponent<Component::Light>())
        {
          Component::Light& l = child.GetComponent<Component::Light>();
          l.mLightIntensity -= (l.mLightIntensity - 0.0f) * Performance::FrameRateController::GetInstance().GetDeltaTime() * speed;
          if (l.mLightIntensity <= 0.04f)
          {
            //std::cout << "Faster?\n";
            l.mLightIntensity = 0.f;
          }
        }
        
      }
    }
  }
  else
    Debug::DebugLogger::GetInstance().LogError("Unable to find entity: [Folder] Lights");

  ECS::Entity es = ECS::EntityManager::GetInstance().GetEntityFromTag("Garden Light");
  if (ECS::Entity(es))
  {
    ECS::Entity(es).SetIsActive(false);
  }
  else
    Debug::DebugLogger::GetInstance().LogError("Unable to find entity: Garden Light");
   
  for (ECS::Entity child : ECS::EntityManager::GetInstance().GetAllEntitiesWithComponents<Component::Light>())
  {
    ECS::Entity parentEntity = ECS::EntityManager::GetInstance().GetParentEntity(child);
    parentEntity.SetIsActive(true);
    ECS::EntityManager::GetInstance().SetChildActiveToFollowParent(parentEntity);
    /*
    std::string n = child.GetTag();
    if (n == "Light")
    {
      child.SetIsActive(true);
    }

    if (n == "PseudoLampBloom") {
      child.SetIsActive(true);
    }
    */
  }



  if (ECS::Entity(cameraEntity) && ECS::Entity{ cameraEntity }.HasComponent<Component::Skybox>()) {
    ECS::Entity{ cameraEntity }.GetComponent<Component::Skybox>().blend -= (ECS::Entity{ cameraEntity }.GetComponent<Component::Skybox>().blend - 0.0f) * Performance::FrameRateController::GetInstance().GetDeltaTime() * speed;
    if (ECS::Entity{ cameraEntity }.GetComponent<Component::Skybox>().blend <= 0.01f)
      ECS::Entity{ cameraEntity }.GetComponent<Component::Skybox>().blend = 0.f;
    return(ECS::Entity{ cameraEntity }.GetComponent<Component::Skybox>().blend <= 0.f);
  }
  else {
    Debug::DebugLogger::GetInstance().LogError("You are trying to change skybox using an entity that does not hav skybox!");
  }
  return true;
}

void Mono::ShowCursor() {
  QUEUE_EVENT(Events::LockMouseEvent, false);
}

void Mono::HideCursor() {
  QUEUE_EVENT(Events::LockMouseEvent, true);
}

void Mono::ChangeToolsPainting() {
  ECS::Entity ToolsUI = ECS::EntityManager::GetInstance().GetEntityFromTag("ToolsPaintingUI");
  ECS::Entity Toolspaint = ECS::EntityManager::GetInstance().GetEntityFromTag("ToolsPainting");
  IGE::Assets::GUID toolsPaintingNight{ IGE::Assets::AssetManager::GetInstance().PathToGUID("..\\Assets\\Textures\\ToolsPaintingNight.png") };
  IGE_ASSETMGR.LoadRef<IGE::Assets::TextureAsset>(toolsPaintingNight);
  if (ECS::EntityManager::GetInstance().IsValidEntity(ToolsUI))
  {
    ToolsUI.GetComponent<Component::Image>().textureAsset = toolsPaintingNight;
  }
 
  for (ECS::Entity& child : ECS::EntityManager::GetInstance().GetChildEntity(Toolspaint))
  {
    if (child.GetTag() == "Painting Plane")
    {
      if (child.HasComponent<Component::Material>())
      {
        Graphics::MaterialTable::GetMaterialByGUID(child.GetComponent<Component::Material>().materialGUID)->SetAlbedoMap(toolsPaintingNight);
      }
      break;
    }
  }

}

void Mono::SpawnToolBox() {
  ECS::Entity toolBox = ECS::EntityManager::GetInstance().GetEntityFromTag("Toolbox");
  if (ECS::EntityManager::GetInstance().IsValidEntity(toolBox))
  {
    toolBox.SetIsActive(true);
    ECS::EntityManager::GetInstance().SetChildActiveToFollowParent(toolBox);
  }

}

void Mono::SpawnOpenDoor() {
    ECS::Entity openDoor = ECS::EntityManager::GetInstance().GetEntityFromTag("OpenDoor");
    ECS::Entity closedDoor = ECS::EntityManager::GetInstance().GetEntityFromTag("ClosedDoor");
    ECS::Entity glowingDoor = ECS::EntityManager::GetInstance().GetEntityFromTag("Puzzle_GlowingDoor");
    if (ECS::EntityManager::GetInstance().IsValidEntity(openDoor) && ECS::EntityManager::GetInstance().IsValidEntity(closedDoor) && ECS::EntityManager::GetInstance().IsValidEntity(glowingDoor))
    {
        openDoor.SetIsActive(true);
        closedDoor.SetIsActive(false);
        glowingDoor.SetIsActive(true);

        ECS::EntityManager::GetInstance().SetChildActiveToFollowParent(openDoor);
        ECS::EntityManager::GetInstance().SetChildActiveToFollowParent(closedDoor);
        ECS::EntityManager::GetInstance().SetChildActiveToFollowParent(glowingDoor);
    }

}

void Mono::SpawnTaraSilhouette() {
    ECS::Entity taraSilhouette = ECS::EntityManager::GetInstance().GetEntityFromTag("TaraSilhouette");
    if (ECS::EntityManager::GetInstance().IsValidEntity(taraSilhouette))
    {
        taraSilhouette.SetIsActive(true);
        ECS::EntityManager::GetInstance().SetChildActiveToFollowParent(taraSilhouette);
    }

}



bool Mono::OnTriggerEnter(ECS::Entity trigger, ECS::Entity other) {
  if (!trigger.HasComponent<Component::BoxCollider>()) {
    Debug::DebugLogger::GetInstance().LogError("You are trying to check collision using an entity that does not have collider!");
  }

  if (!trigger.GetComponent<Component::BoxCollider>().sensor) {
    Debug::DebugLogger::GetInstance().LogWarning("You are trying to check collision using an entity that has sensor turned off!");
  }

  return IGE::Physics::PhysicsSystem::GetInstance().get()->OnTriggerEnter(trigger, other);
}

bool Mono::OnTriggerExit(ECS::Entity trigger, ECS::Entity other) {
  if (!trigger.HasComponent<Component::BoxCollider>()) {
    Debug::DebugLogger::GetInstance().LogError("You are trying to check collision using an entity that does not have collider!");
  }

  if (!trigger.GetComponent<Component::BoxCollider>().sensor) {
    Debug::DebugLogger::GetInstance().LogWarning("You are trying to check collision using an entity that has sensor turned off!");
  }

  return IGE::Physics::PhysicsSystem::GetInstance().get()->OnTriggerExit(trigger, other);
}

MonoArray* Mono::GetContactPoints(ECS::Entity entity1, ECS::Entity entity2)
{
    auto contactPoints{ IGE::Physics::PhysicsSystem::GetInstance()->GetContactPoints(entity1, entity2) };
    //Get the MonoClass for your C# ContactPoint struct.
    MonoClass* contactPointClass = ScriptManager::mMonoUtilsMap["ContactPoint"];
    if (!contactPointClass)
    {
        // Handle error: you might log and return nullptr
        Debug::DebugLogger::GetInstance().LogError("No such class ContactPoint");
        return nullptr;
    }

    // Get the current app domain (assumed to be stored in your ScriptManager)
    MonoDomain* domain = ScriptManager::mAppDomain;  // adjust if needed

    // Create a new managed array of the ContactPoint struct
    MonoArray* managedArray = mono_array_new(domain, contactPointClass, contactPoints.size());

    // For each contact point, fill in a ContactPoint instance and write it into the array.
    for (size_t i = 0; i < contactPoints.size(); ++i)
    {
        // Create a local ContactPoint instance.
        physx::PxContactPairPoint cp;
        cp.position[0] = contactPoints[i].position.x;
        cp.position[1] = contactPoints[i].position.y;
        cp.position[2] = contactPoints[i].position.z;

        cp.separation = contactPoints[i].separation;

        cp.normal[0] = contactPoints[i].normal.x;
        cp.normal[1] = contactPoints[i].normal.y;
        cp.normal[2] = contactPoints[i].normal.z;

        cp.internalFaceIndex0 = contactPoints[i].internalFaceIndex0;

        cp.impulse[0] = contactPoints[i].impulse.x;
        cp.impulse[1] = contactPoints[i].impulse.y;
        cp.impulse[2] = contactPoints[i].impulse.z;

        cp.internalFaceIndex1 = contactPoints[i].internalFaceIndex1;

        // Obtain a pointer to the array element�s storage.
        // Calculate the element size from the MonoClass information.
        void* elementAddr = mono_array_addr_with_size(managedArray, mono_class_value_size(contactPointClass, nullptr), i);
        // Copy our local instance into the managed array.
        memcpy(elementAddr, &cp, sizeof(physx::PxContactPairPoint));
    }

    return managedArray;
}

//only works if there is a box, sphere, capsule, or rb
float Mono::GetShortestDistance(ECS::Entity::EntityID id1, ECS::Entity::EntityID id2) {
    ECS::Entity e1{ id1 };
    ECS::Entity e2{ id2 };
    if ((e1.HasComponent<Component::BoxCollider>() || e1.HasComponent<Component::CapsuleCollider>() || e1.HasComponent<Component::SphereCollider>() || e1.HasComponent<Component::RigidBody>()) && 
        (e2.HasComponent<Component::BoxCollider>() || e2.HasComponent<Component::CapsuleCollider>() || e2.HasComponent<Component::SphereCollider>() || e2.HasComponent<Component::RigidBody>())) {
        return IGE::Physics::PhysicsSystem::GetInstance()->GetShortestDistance(e1, e2);
    }
    return -1.f;
}

void Mono::SetBloomIntensity(ECS::Entity::EntityID entity, float intensity) {
  if (ECS::Entity(entity))
  {
    if (ECS::Entity(entity).HasComponent<Component::Bloom>())
      ECS::Entity(entity).GetComponent<Component::Bloom>().intensity = intensity;
    else
      Debug::DebugLogger::GetInstance().LogError("SetBloomIntensity: Entity " + ECS::Entity(entity).GetTag() + " does not have a Bloom component");
  }
  else {
    Debug::DebugLogger::GetInstance().LogError("SetBloomIntensity: No entity with ID: " + std::to_string(static_cast<uint32_t>(entity)));
  }
}

float Mono::GetBloomIntensity(ECS::Entity::EntityID entity) {
  if (ECS::Entity(entity)) {
    if (ECS::Entity(entity).HasComponent<Component::Bloom>())
      return ECS::Entity(entity).GetComponent<Component::Bloom>().intensity;
    else
      Debug::DebugLogger::GetInstance().LogError("GetBloomIntensity: Entity " + ECS::Entity(entity).GetTag() + " does not have a Bloom component");
  }
  else {
    Debug::DebugLogger::GetInstance().LogError("GetBloomIntensity: No entity with ID: " + std::to_string(static_cast<uint32_t>(entity)));
  }
  return 0.0f;
}

void Mono::SetLightIntensity(ECS::Entity::EntityID entity, float intensity) {
  if (ECS::Entity(entity))
  {
    if (ECS::Entity(entity).HasComponent<Component::Light>())
      ECS::Entity(entity).GetComponent<Component::Light>().mLightIntensity = intensity;
    else
      Debug::DebugLogger::GetInstance().LogError("SetLightIntensity: Entity " + ECS::Entity(entity).GetTag() + " does not have a Light component");
  }
  else {
    Debug::DebugLogger::GetInstance().LogError("SetLightIntensity: No entity with ID: " + std::to_string(static_cast<uint32_t>(entity)));
  }
}

float Mono::GetLightIntensity(ECS::Entity::EntityID entity) {
  if (ECS::Entity(entity)) {
    if (ECS::Entity(entity).HasComponent<Component::Light>())
      return ECS::Entity(entity).GetComponent<Component::Light>().mLightIntensity;
    else
      Debug::DebugLogger::GetInstance().LogError("GetLightIntensity: Entity " + ECS::Entity(entity).GetTag() + " does not have a Light component");
  }
  else {
    Debug::DebugLogger::GetInstance().LogError("GetLightIntensity: No entity with ID: " + std::to_string(static_cast<uint32_t>(entity)));
  }
  return 0.0f;
}

void Mono::PauseGame() {
  gIsGamePaused = true;
}

void Mono::ResumeGame() {
  gIsGamePaused = false;
}

bool Mono::GetIsPaused() {
  return gIsGamePaused;
}

void Mono::SetCanvasTransitionProgress(ECS::Entity::EntityID canvasEntity, float progress) {
  if (ECS::Entity(canvasEntity)) {
    if (ECS::Entity(canvasEntity).HasComponent<Component::Canvas>())
      ECS::Entity(canvasEntity).GetComponent<Component::Canvas>().transitionProgress = progress;

    else
      Debug::DebugLogger::GetInstance().LogError("SetCanvasTransitionProgress: Entity " + ECS::Entity(canvasEntity).GetTag() + " does not have a Canvas component");
  }

  else {
    Debug::DebugLogger::GetInstance().LogError("SetCanvasTransitionProgress: No entity with ID: " + std::to_string(static_cast<uint32_t>(canvasEntity)));
  }
}

void Mono::EnableCanvasTransition(ECS::Entity::EntityID canvasEntity, bool isEnabled) {
  if (ECS::Entity(canvasEntity)) {
    if (ECS::Entity(canvasEntity).HasComponent<Component::Canvas>())
      ECS::Entity(canvasEntity).GetComponent<Component::Canvas>().hasTransition = isEnabled;

    else
      Debug::DebugLogger::GetInstance().LogError("EnableCanvasTransition: Entity " + ECS::Entity(canvasEntity).GetTag() + " does not have a Canvas component");
  }

  else {
    Debug::DebugLogger::GetInstance().LogError("EnableCanvasTransition: No entity with ID: " + std::to_string(static_cast<uint32_t>(canvasEntity)));
  }
}

void Mono::SetCanvasTransitionType(ECS::Entity::EntityID canvasEntity, int transitionType) {
  if (ECS::Entity(canvasEntity)) {
    if (ECS::Entity(canvasEntity).HasComponent<Component::Canvas>())
      ECS::Entity(canvasEntity).GetComponent<Component::Canvas>().transitionType = static_cast<Component::Canvas::TransitionType>(transitionType);

    else
      Debug::DebugLogger::GetInstance().LogError("SetCanvasTransitionType: Entity " + ECS::Entity(canvasEntity).GetTag() + " does not have a Canvas component");
  }

  else {
    Debug::DebugLogger::GetInstance().LogError("SetCanvasTransitionType: No entity with ID: " + std::to_string(static_cast<uint32_t>(canvasEntity)));
  }
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
  return (mono_string_new(sm->mAppDomain, str.c_str()));

}