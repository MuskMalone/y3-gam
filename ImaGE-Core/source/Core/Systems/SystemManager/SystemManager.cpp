/*!*********************************************************************
\file   SystemManager.cpp
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Definition of cclass SystemManager, which holds the instance
        to all systems used in the engine. Responsible for updating
        them every game loop.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include "pch.h"
#include "SystemManager.h"

namespace Systems {

  void SystemManager::InitSystems() {
    for (SystemPtr const& system : mSystems) {
      system->Start();
    }
  }

  void SystemManager::UpdateSystems() {
    Performance::FrameRateController& frc{ Performance::FrameRateController::GetInstance() };
    for (SystemPtr const& system : mSystems) {
      frc.StartSystemTimer();

      system->Update();

      frc.EndSystemTimer(system->GetName());
    }
  }

  void SystemManager::PausedUpdateSystems(std::initializer_list<const char*> const& names) {
    Performance::FrameRateController& frc{ Performance::FrameRateController::GetInstance() };

    for (const char* name : names) {
#ifdef _DEBUG
      if (!mNameToSystem.contains(name)) {
        throw Debug::Exception<SystemManager>(Debug::LVL_ERROR, Msg(std::string("Trying to update unregistered system of type ") + name));
      }
#endif

      frc.StartSystemTimer();
      mNameToSystem[name]->PausedUpdate();
      frc.EndSystemTimer(name);
    }
  }

  SystemManager::~SystemManager() {
    for (SystemPtr const& system : mSystems) {
      system->Destroy();
    }
  }

} // namespace Systems
