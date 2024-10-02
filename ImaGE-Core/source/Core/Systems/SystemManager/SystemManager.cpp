#include "pch.h"
#include "SystemManager.h"

namespace Systems {

  void SystemManager::InitSystems() {
    for (auto const& [name, system] : mSystems) {
      system->Start();
    }
  }

  void SystemManager::UpdateSystems() {
    Performance::FrameRateController& frc{ Performance::FrameRateController::GetInstance() };
    for (auto const&[name, system] : mSystems) {
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
    for (auto const& [name, system] : mSystems) {
      system->Destroy();
    }
  }

} // namespace Systems
