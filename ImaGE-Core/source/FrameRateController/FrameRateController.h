#pragma once
#include <chrono>
#include <map>
#include <Singleton/ThreadSafeSingleton.h>

// FrameRateController
#define IGE_FRC Performance::FrameRateController::GetInstance()

namespace Performance {
  class FrameRateController : public ThreadSafeSingleton<FrameRateController> {
  public:
    using TimeType = float;
    using TimeFormat = std::chrono::microseconds;

    FrameRateController(float targetFPS = 120.f, float fpsCalculationInterval = 0.05f, bool vsyncEnabled = false);

    void Start();
    void End();

    // System Timers
    void StartSystemTimer();
    void EndSystemTimer(std::string const& systemName);
    std::map<std::string, FrameRateController::TimeFormat> const& GetSystemTimerMap() const noexcept;

    // Getters
    TimeType GetFPSCalculationInterval() const noexcept;
    TimeType GetTargetFPS() const noexcept;
    TimeType GetDeltaTime() const noexcept;
    TimeType GetFPS() const noexcept;
    bool GetVsyncFlag() const noexcept;
    unsigned int GetFrameCounter() const noexcept;
    TimeType GetTime() const noexcept;

    // Setters
    void SetFPSCalculationInterval(float fpsCalculationInterval);
    void SetTargetFPS(float target);
    void SetVsync(bool vsyncEnabled);



    void Reset();

  private:
    std::chrono::time_point<std::chrono::high_resolution_clock> mSystemTimeStart{};
    std::map<std::string, TimeFormat> mSystemTimerMap;

    bool mVsyncEnabled{};
    TimeType mAccumulatedFPS{};
    TimeType mTotalTime{};
    TimeType mCurrFrameTime{}, mNewFrameTime{}, mDeltaTime{};
    TimeType mFPSTimer{}, mCurrFPS{};
    TimeType mTargetFPS{};
    TimeType mTargetFrameTime{};
    TimeType mFPSCalculationInterval{};
    unsigned mFrameCounter{};
  };
} // namespace Performance
