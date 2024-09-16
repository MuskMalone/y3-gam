/*!*********************************************************************
\file       ObjectFactory.h
\author     chengen.lau\@digipen.edu
\date       15-September-2024
\brief
  Contains the definition of the ObjectFactory singleton class, which
  encapsulates functions and data members to facilitate the creation
  of objects in the scene. The class serves as a middle-man between
  serialization and the actual entities in the scene. It is responsible
  for the creation/cloning of entities and adding of components through
  the ECS.

  On load, the ObjectFactory holds the deserialized data for the scene.
  Upon reloading, the objects are loaded from here without the need to
  deserialize again. Only when changing scenes will a full reload be
  required.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include "Singleton.h"
#include "VariantEntity.h"
#include <Core/Entity.h>

namespace Reflection
{

  class ObjectFactory : public Singleton<ObjectFactory>
  {
  public:
    using EntityData = std::pair<ECS::EntityManager::EntityID, VariantEntity>;
    using EntityDataContainer = std::vector<EntityData>;

    /*!*********************************************************************
    \brief
      Creates a copy of an entity along with its components. This also
      duplicates all child entities, which is done through recursive calls
      to this function
    \param entity
      The entity to duplicate
    \param parent
      The parent of the entity. It is set to entt::null by default,
      indicating no parent.
    \return
    ************************************************************************/
    void CloneObject(ECS::Entity const& entity, ECS::Entity const& parent = {}) const;

    /*!*********************************************************************
    \brief
      Creates the entities for the scene with the data stored.
    ************************************************************************/
    void InitScene();

    /*!*********************************************************************
    \brief
      Deserializes data from the scene file into the container held by the
      class
    \param filePath
      The path of the file
    ************************************************************************/
    void LoadEntityData(std::string const& filePath);

    /*!*********************************************************************
    \brief
      Extracts a component from an rttr::variant and adds it to an entity
    \param entity
      The entity to add to
    \param compVar
      The component to add
   ************************************************************************/
    void AddComponentToEntity(ECS::Entity entity, rttr::variant const& compVar) const;

    /*!*********************************************************************
    \brief
      Adds all components within a vector of rttr::variants to an entity
    \param id
      Id of the entity
    \param components
      The vector of components
    ************************************************************************/
    void AddComponentsToEntity(ECS::Entity id, std::vector<rttr::variant> const& components) const;

    /*!*********************************************************************
    \brief
      Removes a component from an entity given the rttr::type
    \param compType
      The type of the component
    ************************************************************************/
    void RemoveComponentFromEntity(ECS::Entity entity, rttr::type compType) const;

    /*!*********************************************************************
    \brief
      Returns the component of an entity given the Enumeration type. The
      component is returned in the form of an rttr::variant containing
      a shared_ptr to the object
    \param id
      The id of the entity
    \param type
      The rttr::type of the component
    \return
      An rttr::variant of the component
    ************************************************************************/
    rttr::variant GetEntityComponent(ECS::Entity const& id, rttr::type const& compType) const;

    /*!*********************************************************************
    \brief
      Returns the components of a given entity in the form of a vector of
      rttr::variants, with each variant holding a component.
    \param id
      The id of the entity
    \return
      An std::vector of components in the form of rttr::variants
    ************************************************************************/
    std::vector<rttr::variant> GetEntityComponents(ECS::Entity const& id) const;

  private:
    EntityDataContainer mRawEntities;   // Container of deserialized entity data in format <id, data>
  };

} // namespace Reflection