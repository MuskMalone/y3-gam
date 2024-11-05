/*!*********************************************************************
\file   RegisterClasses.cpp
\date   15-September-2024
\brief  This file handles the registration of custom classes/structs
        used in our engine to be recognized by RTTR library. Anything
        that needs to be serialized/deserialized needs to be registered.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <rttr/registration>
#include <Prefabs/Prefab.h>
#include <PxPhysicsAPI.h>
#include <Scripting/ScriptInstance.h>
#include <Serialization/JsonKeys.h>
#include <Reflection/ProxyScript.h>
#include <Asset/AssetMetadata.h>
#include <Asset/Assetables/Assetables.h>
#include <Core/Systems/LayerSystem/LayerSystem.h>
#include <Core/GUID.h>
#include <Audio/AudioManager.h>
#include <Core/Components/AudioSource.h>
#include <Graphics/PostProcessing/PostProcessingManager.h>

static void rttr_auto_register_reflection_function_(); namespace {
  struct rttr__auto__register__ {
    rttr__auto__register__() {
      rttr_auto_register_reflection_function_();
    }
  };
} static const rttr__auto__register__ auto_register__19; static void rttr_auto_register_reflection_function_()
{
  /* ------------------- GLM ------------------- */
  rttr::registration::class_<glm::vec2>("Vec2")
    .constructor<>()(rttr::policy::ctor::as_object)
    .property("x", &glm::vec2::x)
    .property("y", &glm::vec2::y);
  rttr::registration::class_<glm::dvec2>("dVec2")
    .constructor<>()(rttr::policy::ctor::as_object)
    .property("x", &glm::dvec2::x)
    .property("y", &glm::dvec2::y);

  rttr::registration::class_<glm::vec3>("Vec3")
    .constructor<>()(rttr::policy::ctor::as_object)
    .property("x", &glm::vec3::x)
    .property("y", &glm::vec3::y)
    .property("z", &glm::vec3::z);
  rttr::registration::class_<glm::dvec3>("dVec3")
    .constructor<>()(rttr::policy::ctor::as_object)
    .property("x", &glm::dvec3::x)
    .property("y", &glm::dvec3::y)
    .property("z", &glm::dvec3::z);

  rttr::registration::class_<glm::vec4>("Vec4")
    .constructor<>()(rttr::policy::ctor::as_object)
    .property("x", &glm::vec4::x)
    .property("y", &glm::vec4::y)
    .property("z", &glm::vec4::z)
    .property("w", &glm::vec4::w);
  rttr::registration::class_<glm::dvec4>("dVec4")
    .constructor<>()(rttr::policy::ctor::as_object)
    .property("x", &glm::dvec4::x)
    .property("y", &glm::dvec4::y)
    .property("z", &glm::dvec4::z)
    .property("w", &glm::dvec4::w);

  rttr::registration::class_<glm::quat>("GLMQuat")
    .constructor<>()(rttr::policy::ctor::as_object)
    .property("x", &glm::quat::x)
    .property("y", &glm::quat::y)
    .property("z", &glm::quat::z)
    .property("w", &glm::quat::w);
  
  /* ------------------- Other ------------------- */
  rttr::registration::class_<rttr::type>("RttrType");

  rttr::registration::class_<std::vector<uint64_t>>("std::vector<uint64_t>")
    .constructor<>()(rttr::policy::ctor::as_object);
  
  rttr::registration::class_<physx::PxVec3>("PxVec3")
    .constructor<>()(rttr::policy::ctor::as_object)
    .property("x", &physx::PxVec3::x)
    .property("y", &physx::PxVec3::y)
    .property("z", &physx::PxVec3::z);

  {
    using T = Systems::LayerSystem::LayerData;
    rttr::registration::class_<T>("LayerData")
      .constructor<>()
      .property("layerNames", &T::layerNames)
      .property("layerVisibility", &T::layerVisibility)
      .property("collisionMatrix", &T::collisionMatrix);
  }

  rttr::registration::class_<physx::PxQuat>("PxQuat")
      .constructor<>()(rttr::policy::ctor::as_object)
      .property("x", &physx::PxQuat::x)
      .property("y", &physx::PxQuat::y)
      .property("z", &physx::PxQuat::z)
      .property("w", &physx::PxQuat::w);

  {
    using T = IGE::Core::GUID<IGE::Assets::AssetGUIDTag>;
    rttr::registration::class_<T>("AssetGUID")
      //.property("Seed", &T::mSeed)
      .property("ID", &T::mID);
  }

  if (IGE::Application::GetImGuiEnabled()) {
      rttr::registration::class_<std::pair<std::string, unsigned>>("StringUnsignedPair")
          .property("first", &std::pair<std::string, unsigned>::first)
          .property("second", &std::pair<std::string, unsigned>::second);
  }

  {
    using T = IGE::Assets::AssetMetadata;
    rttr::registration::class_<T::AssetProps>("AssetProps")
      .constructor<>()(rttr::policy::ctor::as_object);
    rttr::registration::class_<T::AssetCategory>("AssetCategory")
      .constructor<>()(rttr::policy::ctor::as_object);
    rttr::registration::class_<T::IGEProjProperties>("IGEProjProperties")
      .constructor<>()(rttr::policy::ctor::as_object);

    /* ------------------- Assets ------------------- */
    rttr::registration::class_<T>("AssetMetadata")
      .constructor<>()(rttr::policy::ctor::as_object)
      .property("Metadata", &T::mAssetProperties);
  }

  //each kind of asset type reflects the corresponding folder in ../Assets
  rttr::registration::class_<IGE::Assets::ModelAsset>("Models");
  rttr::registration::class_<IGE::Assets::TextureAsset>("Textures");
  rttr::registration::class_<IGE::Assets::PrefabAsset>("Prefabs");
  rttr::registration::class_<IGE::Assets::AudioAsset>("Audio");
  rttr::registration::class_<IGE::Assets::FontAsset>("Fonts");
  rttr::registration::class_<IGE::Assets::ShaderAsset>("PostProcessing");

  /* ------------------- Audio ------------------- */
  {
      // Register the SoundInvokeSetting struct
      rttr::registration::class_<IGE::Audio::SoundInvokeSetting>("SoundInvokeSetting")
          .constructor<>()(rttr::policy::ctor::as_object)
          .property("position", &IGE::Audio::SoundInvokeSetting::position)
          .property("volume", &IGE::Audio::SoundInvokeSetting::volume)
          .property("pitch", &IGE::Audio::SoundInvokeSetting::pitch)
          .property("pan", &IGE::Audio::SoundInvokeSetting::pan)
          .property("mute", &IGE::Audio::SoundInvokeSetting::mute)
          .property("loop", &IGE::Audio::SoundInvokeSetting::loop)
          .property("playOnAwake", &IGE::Audio::SoundInvokeSetting::playOnAwake)
          .property("dopplerLevel", &IGE::Audio::SoundInvokeSetting::dopplerLevel)
          .property("minDistance", &IGE::Audio::SoundInvokeSetting::minDistance)
          .property("maxDistance", &IGE::Audio::SoundInvokeSetting::maxDistance)
          .property("rolloffType", &IGE::Audio::SoundInvokeSetting::rolloffType); // As reference for mutable property
      rttr::registration::class_<Component::AudioSource::AudioInstance>("AudioInstance")
          .constructor<>()(rttr::policy::ctor::as_object)
          .property("guid", &Component::AudioSource::AudioInstance::guid)
          .property("playSettings", &Component::AudioSource::AudioInstance::playSettings);
  }
  /* ------------------ Shaders ---------------------*/
  rttr::registration::class_<Graphics::PostProcessingManager::PostProcessingConfigs>("PostProcessingConfigs")
      .constructor<>()(rttr::policy::ctor::as_object)
      .property("configs", &Graphics::PostProcessingManager::PostProcessingConfigs::mConfigs);
  /* ------------------- Script ------------------- */
  {
    using T = Reflection::ProxyScript;
    rttr::registration::class_<T>("Script")
      .constructor<>()
      .property("scriptName", &T::scriptName)
      .property("scriptFieldProxyList", &T::scriptFieldProxyList);
  }
  {
    using T = Mono::ScriptInstance;
    rttr::registration::class_<T>("ScriptInstance")
      .property(JSON_SCRIPT_NAME_KEY, &T::mScriptName)
      .property(JSON_SCRIPT_FIELD_LIST_KEY, &T::mScriptFieldInstList);
  }

  rttr::registration::class_<Mono::ScriptFieldInfo>("ScriptFieldInfo")
    .property("fieldName", &Mono::ScriptFieldInfo::mFieldName);
  {
    using T = Mono::ScriptFieldInstance<int>;
    rttr::registration::class_<T>("System.Int32")
      .constructor<>()
      .property("data", &T::mData)
      .property(JSON_SCRIPT_FILIST_TYPE_KEY, &T::mType)
      .property("scriptField", &T::mScriptField);
  }
  {
    using T = Mono::ScriptFieldInstance<float>;
    rttr::registration::class_<T>("System.Single")
      .constructor<>()
      .property("data", &T::mData)
      .property(JSON_SCRIPT_FILIST_TYPE_KEY, &T::mType)
      .property("scriptField", &T::mScriptField);
  }
  {
    using T = Mono::ScriptFieldInstance<double>;
    rttr::registration::class_<T>("System.Double")
      .constructor<>()
      .property("data", &T::mData)
      .property(JSON_SCRIPT_FILIST_TYPE_KEY, &T::mType)
      .property("scriptField", &T::mScriptField);
  }
  {
    using T = Mono::ScriptFieldInstance<unsigned>;
    rttr::registration::class_<T>("System.UInt32")
      .constructor<>()
      .property("data", &T::mData)
      .property(JSON_SCRIPT_FILIST_TYPE_KEY, &T::mType)
      .property("scriptField", &T::mScriptField);
  }
  {
    using T = Mono::ScriptFieldInstance<std::vector<int>>;
    rttr::registration::class_<T>("System.Int32[]")
      .constructor<>()
      .property("data", &T::mData)
      .property(JSON_SCRIPT_FILIST_TYPE_KEY, &T::mType)
      .property("scriptField", &T::mScriptField);
  }
  {
    using T = Mono::ScriptFieldInstance<std::vector<unsigned>>;
    rttr::registration::class_<T>("System.UInt32[]")
      .constructor<>()
      .property("data", &T::mData)
      .property(JSON_SCRIPT_FILIST_TYPE_KEY, &T::mType)
      .property("scriptField", &T::mScriptField);
  }
  {
    using T = Mono::ScriptFieldInstance<std::string>;
    rttr::registration::class_<T>("System.String")
      .constructor<>()
      .property("data", &T::mData)
      .property(JSON_SCRIPT_FILIST_TYPE_KEY, &T::mType)
      .property("scriptField", &T::mScriptField);
  }
  {
    using T = Mono::ScriptFieldInstance<std::vector<std::string>>;
    rttr::registration::class_<T>("System.String[]")
      .constructor<>()
      .property("data", &T::mData)
      .property(JSON_SCRIPT_FILIST_TYPE_KEY, &T::mType)
      .property("scriptField", &T::mScriptField);
  }
  {
    using T = Mono::ScriptFieldInstance<glm::vec3>;
    rttr::registration::class_<T>("ImaGE-Script.Mono.Vec3<System.float>")
      .constructor<>()
      .property("data", &T::mData)
      .property(JSON_SCRIPT_FILIST_TYPE_KEY, &T::mType)
      .property("scriptField", &T::mScriptField);
  }
  {
    using T = Mono::ScriptFieldInstance<glm::dvec3>;
    rttr::registration::class_<T>("ImaGE-Script.Mono.Vec3<System.double>")
      .constructor<>()
      .property("data", &T::mData)
      .property(JSON_SCRIPT_FILIST_TYPE_KEY, &T::mType)
      .property("scriptField", &T::mScriptField);
  }
}
