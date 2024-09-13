#pragma once
#include "Singleton.h"

class FrameRateController : public Singleton <FrameRateController> {
public:
  using TimeType = float;

  void Init(float targetFPS = 120.f, float fpsCalculationInterval = 1.f, bool vsyncEnabled = false);
  void Start();
  void End();
  TimeType GetDeltaTime() const noexcept;
  TimeType GetFPS() const noexcept;

  bool GetVsyncFlag() const noexcept;
  void SetVsync(bool VsyncFlag);

  void Reset();

private:
  bool m_vsyncEnabled{};
  TimeType m_currFrameTime{}, m_newFrameTime{}, m_deltaTime{};
  TimeType m_fpsTimer{}, m_currFPS{};
  TimeType m_targetFPS{};
  TimeType m_targetFrameTime{};
  TimeType m_fpsCalculationInterval{};
  unsigned m_frameCounter{};
};
