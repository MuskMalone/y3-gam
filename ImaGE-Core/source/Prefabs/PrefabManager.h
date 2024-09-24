/*!*********************************************************************
\file   PrefabManager.h
\author chengen.lau\@digipen.edu
\date   14-September-2024
\brief  
  This file contains the definition of PrefabManager singleton.
  It is responsible for the mapping of entities to the prefabs they
  were created from. This is to allow each instance to be updated by
  any changes made to the prefab itself. The functions below are
  used to facilitate the adding and removing of entities to prefabs,
  as well as the updating of components based on prefabs.
  Currently, the PrefabManager attaches a "version" to each prefab so
  that only outdated entities are updated with the prefab's components
  upon loading a scene.
  
  Further improvements can be made so that an entity's component 
  should no longer be updated if it was changed externally through
  inspector.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <Singleton.h>
#include "VariantPrefab.h"
#include <unordered_map>
#include <Core/Entity.h>
#include <optional>
#include <glm/glm.hpp>
#include <Events/EventCallback.h>

namespace Prefabs
{
  class PrefabManager : public Singleton<PrefabManager>
  {
  public:
    using PrefabDataContainer = std::unordered_map<std::string, VariantPrefab>;
    using EntityPrefabMap = std::unordered_map<ECS::EntityManager::EntityID, VariantPrefab::EntityMappings>;

    /*!*********************************************************************
    \brief
      Creates an instance of a prefab
    \param key
      Name of the prefab
    \param pos
      The position to spawn the prefab at
    \return
      Entity id of the instance.
    ************************************************************************/
    ECS::Entity SpawnPrefab(const std::string& key, glm::dvec3 const& pos = {}, bool mapEntity = true);

    /*!*********************************************************************
    \brief
      Subscribes to prefab-related events through the EventManager
    ************************************************************************/
    void Init();

    /*!*********************************************************************
    \brief
      Checks if a particular prefab has been loaded
    \param name
      The name of the prefab
    \return
      True if it has been loaded and false otherwise
    ************************************************************************/
    inline bool IsPrefabLoaded(std::string const& name) const noexcept { return mPrefabs.contains(name); }

    /*!*********************************************************************
    \brief
      Checks if a particular prefab exists
    \param name
      The name of the prefab
    \return
      True if it exists and false otherwise
    ************************************************************************/
    bool DoesPrefabExist(std::string const& name) const;

    /*!*********************************************************************
    \brief
      Gets the deserialized data of a prefab in the form of a VariantPrefab
      object. Throws a GE::Debug::Exception if not found.
    \param name
      The name of the prefab
    \return
      The VariantPrefab object
    ************************************************************************/
    VariantPrefab const& GetVariantPrefab(std::string const& name) const;

    /*!*********************************************************************
    \brief
      Reloads a particular prefab given its name and the file path
    \param name
      The name of the prefab
    \param filePath
      The file to deserialize from
    ************************************************************************/
    void ReloadPrefab(std::string const& name, std::string const& filePath);

    /*!*********************************************************************
    \brief
      Clears the prefab container and loads prefabs from files again
    ************************************************************************/
    void ReloadPrefabs();

    /*!*********************************************************************
    \brief
      Clears the mappings for each prefab instance in the scene
    ************************************************************************/
    inline void ClearMappings() { mEntitiesToPrefabs.clear(); }

    /*!*********************************************************************
    \brief
      Gets the prefab an entity if it was created from one and std::nullopt
      otherwise
    \param entity
      The entity to get the prefab of
    \return
      std::optional containing the prefab an entity was created from
    ************************************************************************/
    std::optional<std::reference_wrapper<PrefabManager::EntityPrefabMap::mapped_type>> GetEntityPrefab(ECS::Entity entity);

    /*!*********************************************************************
    \brief
      Loads a prefab given its name and the file path
    \param name
      The name of the prefab
    \param filePath
      The file to deserialize from
    ************************************************************************/
    void LoadPrefab(std::string const& name, std::string const& filePath);

  private:
    /*!*********************************************************************
    \brief
      This function handles the corresponding events the PrefabManager
      subscribed to.

      UNLOAD_SCENE
        - Clears mEntitiesToPrefabs, which contains the mappings for
          each entity or prefab instance to their prefabs for the scene
      REMOVE_ENTITY
        - Removes the entity entry from the map if the entity is destroyed
      DELETE_PREFAB
        - Deletes the respective entry from mPrefabs map

    \param event
      The event to be handled
    ************************************************************************/
    EVENT_CALLBACK_DECL(HandleEvent);

  public:
    /*---------------------------- EDITOR - ONLY FUNCTIONS ----------------------------*/
#ifndef IMGUI_DISABLE
      /*!*********************************************************************
      \brief
        Assigns a prefab to an entity
      \param entity
        The entity to assign the prefab
      \param prefab
        std::pair containing the prefab name and version
      ************************************************************************/
    void AttachPrefab(ECS::Entity entity, EntityPrefabMap::mapped_type const& prefab);
    void AttachPrefab(ECS::Entity entity, EntityPrefabMap::mapped_type&& prefab);

    /*!*********************************************************************
    \brief
      Unsubscribes an entity from prefab updates
    \param entity
      The entity id
    ************************************************************************/
    void DetachPrefab(ECS::Entity entity);

    /*!*********************************************************************
    \brief
      Creates a VariantPrefab with the current entity. The prefab will be
      a copy of the entity, together with its children.
    \param entity
      The entity to create the prefab from
    \param name
      The name of the prefab to create
    \return
      The VariantPrefab object of an entity
    ************************************************************************/
    VariantPrefab CreateVariantPrefab(ECS::Entity entity, std::string const& name);

    /*!*********************************************************************
    \brief
      This function creates a prefab from an entity's components. If the
      path argument is not provided, it will default to the Prefabs
      directory based on the Assets file path from the asset manager.
    \param entity
      The entity to create the prefab from
    \param name
      The name of the new prefab
    \param path
      The path to save the file. Will be automatically generated by default
    ************************************************************************/
    void CreatePrefabFromEntity(ECS::Entity const& entity, std::string const& name, std::string const& path = {});

    /*!*********************************************************************
     \brief
       Updates a prefab and saves it to file after modification. The current
       VariantPrefab in the prefab manager is replaced with a new copy and
       updated with any removed objects passed from the prefab editor.
     \param prefabInstance
       The entity ID of the prefab instance
     \param removedChildren
       The vector of SubDataIds of removed children
     \param removedComponents
       The vector of removed components along with the respective SubDataId
     \param filepath
       The path to save the prefab to
    ************************************************************************/
    void UpdatePrefabFromEditor(ECS::Entity prefabInstance, std::vector<Prefabs::PrefabSubData::SubDataId> const& removedChildren,
      std::vector<std::pair<Prefabs::PrefabSubData::SubDataId, rttr::type>> const& removedComponents, std::string const& filepath);

    /*!*********************************************************************
    \brief
      This function updates all entities associated with the given prefab
    \param prefab
      The prefab to update all entities with
    \return
      True if any instance was updated and false otherwise
    ************************************************************************/
    bool UpdateEntitiesFromPrefab(std::string const& prefab);

    /*!*********************************************************************
    \brief
      This function updates all entities in the map based on their
      respective prefabs
    \return
      True if any instance was updated and false otherwise
    ************************************************************************/
    bool UpdateAllEntitiesFromPrefab();

#endif

  private:
    EntityPrefabMap mEntitiesToPrefabs;
    PrefabDataContainer mPrefabs;  // Map of deserialized prefab data in format <name, data>
  };
}
