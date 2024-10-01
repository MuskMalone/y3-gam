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
#include "Prefab.h"
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
    using PrefabDataContainer = std::unordered_map<std::string, Prefab>;
    using EntityPrefabMap = std::unordered_map<ECS::EntityManager::EntityID, Prefab::EntityMappings>;

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
      Gets the deserialized data of a prefab in the form of a Prefab
      object. Throws a GE::Debug::Exception if not found.
    \param name
      The name of the prefab
    \return
      The Prefab object
    ************************************************************************/
    Prefab const& GetVariantPrefab(std::string const& name) const;

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
      Loads a prefab given its name and the file path
    \param name
      The name of the prefab
    \param filePath
      The file to deserialize from
    ************************************************************************/
    void LoadPrefab(std::string const& name);
    
    /*!*********************************************************************
    \brief
      Clears all data from the PrefabManager
    ************************************************************************/
    void Shutdown();
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
    /*!*********************************************************************
    \brief
      Creates an instance of a prefab and returns how each entity is mapped
      to the subdata
    \param key
      Name of the prefab
    \param pos
      The position to spawn the prefab at
    \return
      Entity id of the instance and the mappings in the form of a pair
    ************************************************************************/
    std::pair<ECS::Entity, Prefabs::Prefab::EntityMappings> SpawnPrefabAndMap(const std::string& key,
      glm::dvec3 const& pos = {}, bool mapEntity = true);

    /*!*********************************************************************
    \brief
      Creates a Prefab with the current entity. The prefab will be
      a copy of the entity, together with its children.
    \param entity
      The entity to create the prefab from
    \param name
      The name of the prefab to create
    \return
      The Prefab object of an entity
    ************************************************************************/
    Prefab CreateVariantPrefab(ECS::Entity entity, std::string const& name);

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
       Prefab in the prefab manager is replaced with a new copy and
       updated with any removed objects passed from the prefab editor.
     \param prefabInstance
       The entity ID of the prefab instance
     \param mappings
       The mappings of entity to subdataid when the prefab was created
     \param key
       The key of the prefab
     \param filepath
       The path to save the prefab to
    ************************************************************************/
    void UpdatePrefabFromEditor(ECS::Entity prefabInstance, std::string const& key,
      Prefabs::Prefab::EntityMappings& mappings, std::string const& filePath);

  private:
    PrefabDataContainer mPrefabs;  // Map of deserialized prefab data in format <name, data>
  };
}
