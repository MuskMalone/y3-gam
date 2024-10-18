/*!*********************************************************************
\file   SystemManager.h
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Definition of cclass SystemManager, which holds the instance
        to all systems used in the engine. Responsible for updating
        them every game loop.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <Core/Systems/System.h>

namespace Systems {
  class SystemManager
  {
  public:
    /*!*********************************************************************
    \brief
      Registers a system into the SystemManager
    \param name
      The name of the system
    ************************************************************************/
    template <typename T>
    void RegisterSystem(const char* name);
    
    /*!*********************************************************************
    \brief
      Gets a system from the SystemManager
    \return
      shared_ptr to the system
    ************************************************************************/
    template <typename T>
    std::shared_ptr<T> GetSystem() const;

    /*!*********************************************************************
    \brief
      Initializes all systems held by the class
    ************************************************************************/
    void InitSystems();

    /*!*********************************************************************
    \brief
      Updates all systems specified as template arguments
    ************************************************************************/
    template <typename... Systems>
    void UpdateSelectedSystems();

    /*!*********************************************************************
    \brief
      Updates all systems held by the class
    ************************************************************************/
    void UpdateSystems();

    /*!*********************************************************************
    \brief
      Releases all systems held by the class
    ************************************************************************/
    void Shutdown();

  private:
    using SystemPtr = std::shared_ptr<System>;

    std::unordered_map<const char*, SystemPtr> mNameToSystem;
    std::vector<SystemPtr> mSystems;

    /*!*********************************************************************
    \brief
      Updates a system and times it with the frame rate controller
    \param names
      The typeid names of the systems
    ************************************************************************/
    void UpdateSystems(std::initializer_list<const char*> const& names);

    /*!*********************************************************************
    \brief
      Late-updates a system and times it with the frame rate controller
    \param names
      The typeid names of the systems
    ************************************************************************/
    void LateUpdateSystems(std::initializer_list<const char*> const& names);
  };
#include "SystemManager.tpp"
} // namespace Systems
