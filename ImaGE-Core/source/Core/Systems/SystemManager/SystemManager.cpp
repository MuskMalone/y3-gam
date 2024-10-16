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

    // idk if we need this
    //for (auto const& [name, system] : mSystems) {
    //  system->LateUpdate();
    //}
  }

  SystemManager::~SystemManager() {
    for (SystemPtr const& system : mSystems) {
      system->Destroy();
    }
  }

} // namespace Systems
