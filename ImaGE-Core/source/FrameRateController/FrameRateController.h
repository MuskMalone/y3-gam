#pragma once
#include <chrono>
#include <map>
#include "Singleton.h"

class FrameRateController : public Singleton <FrameRateController> {
public:
  using TimeType = float;
  using TimeFormat = std::chrono::microseconds;

  void Init(float targetFPS = 60.f, float fpsCalculationInterval = 1.f, bool vsyncEnabled = false);
  void Start();
  void End();

  // System Timers
  void StartSystemTimer();
  void EndSystemTimer(std::string systemName);
  std::map<std::string, FrameRateController::TimeFormat> const& GetSystemTimerMap() const noexcept;

  // Getters
  TimeType GetFPSCalculationInterval() const noexcept;
  TimeType GetTargetFPS() const noexcept;
  TimeType GetDeltaTime() const noexcept;
  TimeType GetFPS() const noexcept;
  bool GetVsyncFlag() const noexcept;
  
  // Setters
  void SetFPSCalculationInterval(float fpsCalculationInterval);
  void SetTargetFPS(float target);
  void SetVsync(bool vsyncEnabled);

  void Reset();

private:
  std::chrono::time_point<std::chrono::high_resolution_clock> mSystemTimeStart{};
  std::map<std::string, TimeFormat> mSystemTimerMap;

  bool mVsyncEnabled{};
  TimeType mCurrFrameTime{}, mNewFrameTime{}, mDeltaTime{};
  TimeType mFPSTimer{}, mCurrFPS{};
  TimeType mTargetFPS{};
  TimeType mTargetFrameTime{};
  TimeType mFPSCalculationInterval{};
  unsigned mFrameCounter{};
};