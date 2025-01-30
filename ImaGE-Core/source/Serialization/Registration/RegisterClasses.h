/*!*********************************************************************
\file   RegisterClasses.cpp
\date   15-September-2024
\brief  This file handles the registration of custom classes/structs
        used in our engine to be recognized by RTTR library. Anything
        that needs to be serialized/deserialized needs to be registered.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <rttr/registration>
#include <policy.h>
#include <Serialization/PfbOverridesData.h>
#include <PxPhysicsAPI.h>
#include <Scripting/ScriptInstance.h>
#include <Serialization/JsonKeys.h>
#include <Reflection/ProxyScript.h>
#include <Asset/AssetMetadata.h>
#include <Asset/Assetables/Assetables.h>
#include <Core/LayerManager/LayerManager.h>
#include <Core/GUID.h>
#include <Audio/AudioManager.h>
#include <Core/Components/AudioSource.h>
#include <Graphics/PostProcessing/PostProcessingManager.h>
#include <Graphics/MaterialData.h>
#include <Core/Components/Light.h>
#include <Animation/AnimationData.h>

#define REGISTER_DATA_MEMBER_INST(T, nameStr) rttr::registration::class_<Mono::DataMemberInstance<T>>(nameStr).constructor<>()(rttr::policy::ctor::as_object)\
  .property(JSON_SCRIPT_DMI_DATA_KEY, &Mono::DataMemberInstance<T>::mData)\
  .property(JSON_SCRIPT_DMI_TYPE_KEY, &Mono::DataMemberInstance<T>::mType)\
  .property(JSON_SCRIPT_DMI_SF_KEY, &Mono::DataMemberInstance<T>::mScriptField)

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
    using T = Layers::LayerManager::LayerData;
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
      .constructor<>()(rttr::policy::ctor::as_object)
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

    rttr::registration::class_<T::AssetCategory>("AssetCategory")
      .constructor<>()(rttr::policy::ctor::as_object);
    rttr::registration::class_<T::IGEProjProperties>("IGEProjProperties")
      .constructor<>()(rttr::policy::ctor::as_object);

    /* ------------------- Assets ------------------- */
    rttr::registration::class_<T::AssetProps>("AssetProps")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("metadata", &T::AssetProps::metadata);
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
  rttr::registration::class_<IGE::Assets::MaterialAsset>("Materials");
  rttr::registration::class_<IGE::Assets::AnimationAsset>("Animations");

  /* ------------------- Light ------------------- */
  {
    using T = Component::ShadowConfig;
    rttr::registration::class_<T>("ShadowConfig")
      .constructor<>()(rttr::policy::ctor::as_object)
      .property("centerPos", &T::centerPos)
      .property("softness", &T::softness)
      .property("bias", &T::bias)
      .property("nearPlane", &T::nearPlane)
      .property("farPlane", &T::farPlane)
      .property("scenesBounds", &T::scenesBounds)
      .property("isStatic", &T::isStatic)
      .property("shadowModified", &T::shadowModified)
      .property("customCenter", &T::customCenter);
  }

  {
    using T = Component::LightGlobalProps;
    rttr::registration::class_<T>("LightGlobalProps")
      .constructor<>()(rttr::policy::ctor::as_object)
      .property("ambColor", &T::ambColor)
      .property("ambIntensity", &T::ambIntensity);
  }

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
  /* ------------------ Materials ---------------------*/
  {
      using T = Graphics::MaterialData;
      rttr::registration::class_<T>("MatData")
        .property("name", &T::mName)
        .property("shader", &T::mShaderName)
        .property("albedoColor", &T::mAlbedoColor)
        .property("metalness", &T::mMetalness)
        .property("roughness", &T::mRoughness)
        .property("ao", &T::mAO)
        .property("emission", &T::mEmission)
        .property("transparency", &T::mTransparency)
        .property("tiling", &T::mTiling)
        .property("offset", &T::mOffset)
        .property("albedoMap", &T::mAlbedoMap)
        .property("normalMap", &T::mNormalMap)
        .property("metalnessMap", &T::mMetalnessMap)
        .property("roughnessMap", &T::mRoughnessMap);
  }

  /* ------------------ Prefabs ---------------------*/
  {
    using T = Serialization::PfbOverridesData;
    rttr::registration::class_<T>("PrefabOverridesData")
      .constructor<>()(rttr::policy::ctor::as_object)
      .property(JSON_GUID_KEY, &T::guid)
      .property("subDataId", &T::subDataId)
      .property("componentData", &T::componentData)
      .property("removedComponents", &T::removedComponents);
  }

  /* ------------------- Animation ------------------- */
  {
    using T = Anim::RootKeyframe;
    rttr::registration::class_<T>("RootKeyframe")
      .constructor<>()(rttr::policy::ctor::as_object)
      .property("startPos", &T::startPos)
      .property("startRot", &T::startRot)
      .property("startScale", &T::startScale);
  }

  {
    using T = Anim::Keyframe;
    rttr::registration::class_<T>("Keyframe")
      .constructor<>()(rttr::policy::ctor::as_object)
      .property("type", &T::type)
      .property("startTime", &T::startTime)
      .property("duration", &T::duration);
  }
  {
    using T = Anim::AnimationData;
    rttr::registration::class_<T>("AnimationData")
      .constructor<>()(rttr::policy::ctor::as_object)
      .property("rootKeyframe", &T::rootKeyframe);
  }

  /* ------------------- Script ------------------- */
  {
    using T = Reflection::ProxyScript;
    rttr::registration::class_<T>("Script")
      .constructor<>()(rttr::policy::ctor::as_object)
      .property("scriptName", &T::scriptName)
      .property("scriptFieldProxyList", &T::scriptFieldProxyList);
  }

  {
    using T = Mono::ScriptInstance;
    rttr::registration::class_<T>("ScriptInstance")
      .property(JSON_SCRIPT_ENTITY_ID_KEY, &T::mEntityID)
      .property(JSON_SCRIPT_NAME_KEY, &T::mScriptName)
      .property(JSON_SCRIPT_FIELD_LIST_KEY, &T::mScriptFieldInstList);
  }

  rttr::registration::class_<Mono::ScriptFieldInfo>("ScriptFieldInfo")
    .property("fieldName", &Mono::ScriptFieldInfo::mFieldName);

  rttr::registration::class_<std::vector<MonoObject*>>("MonoObjectVector")
    .property("fieldName", &Mono::ScriptFieldInfo::mFieldName);

  // yay more macros
  REGISTER_DATA_MEMBER_INST(unsigned, "System.UInt32");
  REGISTER_DATA_MEMBER_INST(bool, "System.Boolean");
  REGISTER_DATA_MEMBER_INST(short, "System.Int16");
  REGISTER_DATA_MEMBER_INST(int, "System.Int32");
  REGISTER_DATA_MEMBER_INST(float, "System.Single");
  REGISTER_DATA_MEMBER_INST(double, "System.Double");
  REGISTER_DATA_MEMBER_INST(int64_t, "System.Int64");
  REGISTER_DATA_MEMBER_INST(uint16_t, "System.UInt16");
  REGISTER_DATA_MEMBER_INST(uint64_t, "System.UInt64");
  REGISTER_DATA_MEMBER_INST(std::string, "System.String");
  REGISTER_DATA_MEMBER_INST(glm::vec3, "System.Numerics.Vector3");
  REGISTER_DATA_MEMBER_INST(glm::dvec3, "IGE.Utils.Vec3<System.Double>");
  REGISTER_DATA_MEMBER_INST(std::vector<int>, "System.Int32[]");
  REGISTER_DATA_MEMBER_INST(std::vector<float>, "System.System.Single[]");
  REGISTER_DATA_MEMBER_INST(std::vector<double>, "System.System.Double[]");
  REGISTER_DATA_MEMBER_INST(std::vector<std::string>, "System.String[]");
  REGISTER_DATA_MEMBER_INST(std::vector<unsigned>, "System.UInt32[]");
  REGISTER_DATA_MEMBER_INST(Mono::ScriptInstance, "Image.Mono.ScriptInstance");
  REGISTER_DATA_MEMBER_INST(std::vector<Mono::ScriptInstance>, "Entity[]");
  REGISTER_DATA_MEMBER_INST(std::vector<MonoObject*>, "MonoObjectVec");

}
