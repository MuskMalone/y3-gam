/*!*********************************************************************
\file   System.h
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Defines the interface for all systems to inherit from. They are
        managed by the SystemManager.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <Core/EntityManager.h>
#include <memory>
#include <string>

namespace Systems {
  class System {
  public:
    System(std::string name) : mEntityManager{ ECS::EntityManager::GetInstance() }, mName{ std::move(name) } {}
    virtual ~System() = default;

    /*!*********************************************************************
    \brief
      Initializes the system
    ************************************************************************/
    virtual void Start() {}

    /*!*********************************************************************
    \brief
      Updates the system
    ************************************************************************/
    virtual void Update() {}

    /*!*********************************************************************
    \brief
      Called when the system is destroyed
    ************************************************************************/
    virtual void Destroy() {}
    
    /*!*********************************************************************
    \brief
      Gets the name of the system
    \return
      The name of the system
    ************************************************************************/
    std::string const& GetName() const noexcept { return mName; }

  protected:
    ECS::EntityManager& mEntityManager; // may be bad, but i cant use ptr so

  private:
    std::string const mName;
  };
} // namespace Systems
