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

  void SystemManager::Shutdown() {
    for (SystemPtr const& system : mSystems) {
      system->Destroy();
    }

    mNameToSystem.clear();
    mSystems.clear();
  }

} // namespace Systems