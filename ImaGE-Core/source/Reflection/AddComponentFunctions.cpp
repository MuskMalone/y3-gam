/*!*********************************************************************
\file   AddComponentFunctions.cpp
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Contains the definition of functions used by the ObjectFactory
        to add each component to an entity. This is so that each
        component can be added their own way.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "AddComponentFunctions.h"
#include <Physics/PhysicsSystem.h>
#include <Graphics/Mesh/MeshFactory.h>
#include <Graphics/Mesh/Mesh.h>
#include <Asset/IGEAssets.h>
#include <Events/EventManager.h>
#include <Events/AssetEvents.h>

namespace {
  template <typename T>
  std::string GetRttrTypeString() { return rttr::type::get<T>().get_name().to_string(); }
}

namespace Reflection::ComponentUtils {
  using namespace Component;

#define EXTRACT_RAW_COMP(T, variable) T const& variable{ var.get_type().is_wrapper() ? var.get_wrapped_value<T>() : var.get_value<T>() }
  void AddAudioListener(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(AudioListener, comp);

    entity.EmplaceOrReplaceComponent<AudioListener>(comp);
  }
  void AddAudioSource(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(AudioSource, comp);
      
    // may be bad to try catch in for loop
    // @TODO: should optimize for game build
    bool valid{ true };
    for (auto const& [str, audioInst] : comp.sounds) {
      try {
          IGE_ASSETMGR.LoadRef<IGE::Assets::AudioAsset>(audioInst.guid);
      }
      catch (Debug::ExceptionBase&) {
        IGE_DBGLOGGER.LogError("GUID " + std::to_string(static_cast<uint64_t>(audioInst.guid)) + " of AudioSource component invalid");
        QUEUE_EVENT(Events::GUIDInvalidated, entity, audioInst.guid, GetRttrTypeString<IGE::Assets::AudioAsset>());
        valid = false;
      }
    }

    if (valid) {
      entity.EmplaceOrReplaceComponent<AudioSource>(comp);
    }
  }
  void AddTag(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Tag, comp);

    entity.EmplaceOrReplaceComponent<Tag>(comp);
  }

  void AddTransform(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Transform, comp);

    entity.EmplaceOrReplaceComponent<Transform>(comp);
  }

  void AddLayer(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Layer, comp);

    entity.EmplaceOrReplaceComponent<Layer>(comp);
  }

  void AddBoxCollider(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(BoxCollider, comp);

    IGE::Physics::PhysicsSystem::GetInstance()->AddBoxCollider(entity, false, comp);
  }

  void AddSphereCollider(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(SphereCollider, comp);

    IGE::Physics::PhysicsSystem::GetInstance()->AddSphereCollider(entity, false, comp);
  }

  void AddCapsuleCollider(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(CapsuleCollider, comp);

    IGE::Physics::PhysicsSystem::GetInstance()->AddCapsuleCollider(entity, false, comp);
  }

  void AddRigidBody(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(RigidBody, comp);

    IGE::Physics::PhysicsSystem::GetInstance()->AddRigidBody(entity, comp);
  }

  void AddMaterial(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Material, comp);

    try {
      // let the ctor handle the material asset
      entity.EmplaceOrReplaceComponent<Material>(comp.materialGUID);
    }
    catch (Debug::ExceptionBase&) {
      IGE_DBGLOGGER.LogError("GUID " + std::to_string(static_cast<uint64_t>(comp.materialGUID)) + " of Material component invalid");
      QUEUE_EVENT(Events::GUIDInvalidated, entity, comp.materialGUID, GetRttrTypeString<IGE::Assets::MaterialAsset>());
    }
  }

  void AddMesh(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Mesh, comp);

    try {
      IGE::Assets::GUID const& meshSrc{ comp.isCustomMesh ? IGE_ASSETMGR.LoadRef<IGE::Assets::ModelAsset>(comp.meshSource)
        : IGE_ASSETMGR.LoadRef<IGE::Assets::ModelAsset>(comp.meshName) };

      Mesh copy{ comp };
      copy.meshSource = meshSrc;
      entity.EmplaceOrReplaceComponent<Mesh>(copy);
    }
    catch (Debug::ExceptionBase&) {
      IGE_DBGLOGGER.LogError("Unable to load mesh: " + comp.meshName + "[GUID " + std::to_string(static_cast<uint64_t>(comp.meshSource)) + "]");
      QUEUE_EVENT(Events::GUIDInvalidated, entity, comp.meshSource, GetRttrTypeString<IGE::Assets::ModelAsset>());
    }
  }

  void AddText(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Text, comp);

    entity.EmplaceOrReplaceComponent<Text>(comp);
  }

  void AddScript(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(ProxyScriptComponent, comp);

    entity.EmplaceOrReplaceComponent<Script>(comp, entity);
  }

  void AddLight(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Light, comp);

    entity.EmplaceOrReplaceComponent<Light>(comp);
  }

  void AddCanvas(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Canvas, comp);

    entity.EmplaceOrReplaceComponent<Canvas>(comp);
  }

  void AddImage(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Image, comp);

    try {
      IGE_ASSETMGR.LoadRef<IGE::Assets::TextureAsset>(comp.textureAsset);
      entity.EmplaceOrReplaceComponent<Image>(comp);
    }
    catch (Debug::ExceptionBase&) {
      IGE_DBGLOGGER.LogError("GUID " + std::to_string(static_cast<uint64_t>(comp.textureAsset)) + " of Image component invalid");
      QUEUE_EVENT(Events::GUIDInvalidated, entity, comp.textureAsset, GetRttrTypeString<IGE::Assets::TextureAsset>());
    }
  }

  void AddSprite2D(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Sprite2D, comp);

    try {
      IGE_ASSETMGR.LoadRef<IGE::Assets::TextureAsset>(comp.textureAsset);
      entity.EmplaceOrReplaceComponent<Sprite2D>(comp);
    }
    catch (Debug::ExceptionBase const&) {
      IGE_DBGLOGGER.LogError("GUID " + std::to_string(static_cast<uint64_t>(comp.textureAsset)) + " of Sprite2D component invalid");
      QUEUE_EVENT(Events::GUIDInvalidated, entity, comp.textureAsset, GetRttrTypeString<IGE::Assets::TextureAsset>());
    }
  }

  void AddCamera(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Camera, comp);

    entity.EmplaceOrReplaceComponent<Camera>(comp);
  }

  void AddSkybox(ECS::Entity entity, rttr::variant const& var) {
    EXTRACT_RAW_COMP(Skybox, comp);

    bool success{ true };
    try {
      IGE_ASSETMGR.LoadRef<IGE::Assets::TextureAsset>(comp.tex1);
    }
    catch (Debug::ExceptionBase const&) {
      IGE_DBGLOGGER.LogError("GUID " + std::to_string(static_cast<uint64_t>(comp.tex1)) + " of Skybox component invalid");
      QUEUE_EVENT(Events::GUIDInvalidated, entity, comp.tex1, GetRttrTypeString<IGE::Assets::TextureAsset>());
      success = false;
    }

    try {
      IGE_ASSETMGR.LoadRef<IGE::Assets::TextureAsset>(comp.tex2);
    }
    catch (Debug::ExceptionBase const&) {
      IGE_DBGLOGGER.LogError("GUID " + std::to_string(static_cast<uint64_t>(comp.tex2)) + " of Skybox component invalid");
      QUEUE_EVENT(Events::GUIDInvalidated, entity, comp.tex2, GetRttrTypeString<IGE::Assets::TextureAsset>());
      success = false;
    }

    if (success) {
      entity.EmplaceOrReplaceComponent<Skybox>(comp);
    }
  }

  void AddInteractive(ECS::Entity entity, rttr::variant const& var) {
      EXTRACT_RAW_COMP(Interactive, comp);

      entity.EmplaceOrReplaceComponent<Interactive>(comp);
  }
} // namespace Reflection
