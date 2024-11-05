#include <pch.h>
#include <GLFW/glfw3.h>
#include "FrameRateController.h"

namespace Performance {
  FrameRateController::FrameRateController(float targetFPS, float fpsCalculationInterval, bool vsyncEnabled) {
    mVsyncEnabled = vsyncEnabled;
    SetVsync(mVsyncEnabled);

    mCurrFrameTime = 0.f, mNewFrameTime = 0.f, mDeltaTime = 0.f;
    mFPSTimer = 0.f, mCurrFPS = 0.f;

    mTargetFPS = targetFPS;
    mTargetFrameTime = 1.f / mTargetFPS;
    mFPSCalculationInterval = fpsCalculationInterval;

    mFrameCounter = 0;
  }

  void FrameRateController::Start() {
    mDeltaTime = mNewFrameTime - mCurrFrameTime;
    mCurrFrameTime = mNewFrameTime;

    mFPSTimer += mDeltaTime;

    if (mFPSTimer >= mFPSCalculationInterval) {
      mCurrFPS = static_cast<TimeType>(mFrameCounter) / mFPSCalculationInterval;
      mFPSTimer = 0.f;
    }
  }

  void FrameRateController::End() {
    do {
      mNewFrameTime = static_cast<TimeType>(glfwGetTime());
      mDeltaTime = mNewFrameTime - mCurrFrameTime;
    } while (mDeltaTime < mTargetFrameTime);

    mNewFrameTime = static_cast<TimeType>(glfwGetTime());
    ++mFrameCounter;
  }

  void FrameRateController::StartSystemTimer() {
    mSystemTimeStart = std::chrono::high_resolution_clock::now();
  }

  void FrameRateController::EndSystemTimer(std::string const& systemName) {
    auto endTime{ std::chrono::high_resolution_clock::now() };
    std::map<std::string, TimeFormat>::iterator iter = mSystemTimerMap.find(systemName);

    if (iter != mSystemTimerMap.end())
      iter->second = std::chrono::duration_cast<TimeFormat>(endTime - mSystemTimeStart);
    else
      mSystemTimerMap[systemName] = std::chrono::duration_cast<TimeFormat>(endTime - mSystemTimeStart);

    mSystemTimeStart = endTime;
  }

  std::map<std::string, FrameRateController::TimeFormat> const& FrameRateController::GetSystemTimerMap() const noexcept {
    return mSystemTimerMap;
  }

  FrameRateController::TimeType FrameRateController::GetFPSCalculationInterval() const noexcept {
    return mFPSCalculationInterval;
  }

  FrameRateController::TimeType FrameRateController::GetTargetFPS() const noexcept {
    return mTargetFPS;
  }

  FrameRateController::TimeType FrameRateController::GetDeltaTime() const noexcept {
    return mDeltaTime;
  }

  FrameRateController::TimeType FrameRateController::GetFPS() const noexcept {
    return mCurrFPS;
  }

  bool FrameRateController::GetVsyncFlag() const noexcept {
    return mVsyncEnabled;
  }

  unsigned int FrameRateController::GetFrameCounter() const noexcept {
      return mFrameCounter;
  }

  void FrameRateController::SetFPSCalculationInterval(float fpsCalculationInterval) {
    mFPSCalculationInterval = fpsCalculationInterval;
  }

  void FrameRateController::SetTargetFPS(float target) {
    mTargetFPS = target;
    mTargetFrameTime = 1.f / mTargetFPS;
  }

  void FrameRateController::SetVsync(bool vsyncEnabled) {
    mVsyncEnabled = vsyncEnabled;
    glfwSwapInterval((mVsyncEnabled) ? 1 : 0);
  }


    
  void FrameRateController::Reset() {
    mCurrFrameTime = 0.f;
    mNewFrameTime = 0.f;
    mDeltaTime = 0.f;
    mFPSTimer = 0.f, mCurrFPS = 0.f;
    mFrameCounter = 0;
  }
} // namespace Performance