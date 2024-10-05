/*!*********************************************************************
\file   PerformanceWindow.cpp
\author chengen.lau\@digipen.edu
\date   6-October-2024
\brief  Class encapsulating functions to run the performance window
        editor. Displays information related to the engine's status
        and performance such as FPS, system update times and memory.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include "PerformanceWindow.h"
#include <ImGui/imgui.h>
#include <numeric>

namespace GUI {

  PerformanceWindow::PerformanceWindow(const char* name) : GUIWindow(name),
    mFPSHistory{}, mContainerStartIdx{}, mTimePerUpdate{ 0.5f }, mCurrentFPS{}, mFPSMaxCount{ 100 } {
    mFPSHistory.reserve(sResizeThreshold);
  }

  void PerformanceWindow::Run() {
    ImGui::Begin(mWindowName.c_str());
    UpdateFPS();

    float const sum{ std::accumulate(mFPSHistory.begin(), mFPSHistory.end(), 0.f) };
    std::string const averageStr{ "Average: " + std::to_string(sum / static_cast<float>(mFPSHistory.size())) };
    ImVec2 chartSize{ ImGui::GetContentRegionAvail() };
    chartSize.y /= 2; // take up half the height

    ImGui::Text("FPS Chart");
    ImGui::PlotLines("##FPSChart", mFPSHistory.data(), mFPSMaxCount,
      mContainerStartIdx, averageStr.c_str(), 0.f,
      static_cast<float>(Performance::FrameRateController::GetInstance().GetTargetFPS()) + 10.f, chartSize);

    ImGui::Text(("Current: " + std::to_string(mCurrentFPS)).c_str());

    ImGui::Text("Time per Update");
    ImGui::SliderFloat("##TimePerUpdate", &mTimePerUpdate, 0.1f, 5.f);

    ImGui::Text("Width");
    if (ImGui::SliderInt("##FPSMaxCount", &mFPSMaxCount, 2, 100))
    {
      if (mFPSMaxCount > mFPSHistory.size())
      {
        mFPSHistory.resize(mFPSMaxCount);
        mFPSHistory.reserve(sResizeThreshold);
      }
      else if (mFPSMaxCount < mFPSHistory.size())
      {
        mFPSHistory.erase(mFPSHistory.begin(), mFPSHistory.end() - mFPSMaxCount);
      }
    }

    ImGui::End();
  }

  void PerformanceWindow::UpdateFPS() {
    static float timeElapsed{};
    Performance::FrameRateController& frc{ Performance::FrameRateController::GetInstance() };

    if (timeElapsed >= mTimePerUpdate)
    {
      // because we constantly have to discard the oldest value,
      // the vector has to remove contents from the beginning.
      // i don't want to erase the first element every frame (since it involves moving each subsequent element forward in the vector)
      // so we'll increment an index to read the next n values instead
      // if we hit the threshold, construct a new vector with the last n values
      if (static_cast<int>(mFPSHistory.size()) >= sResizeThreshold)
      {
        mFPSHistory = { mFPSHistory.begin() + mContainerStartIdx, mFPSHistory.end() };
        mFPSHistory.reserve(sResizeThreshold);
        mContainerStartIdx = 0; // reset the idx
      }
      mCurrentFPS = static_cast<float>(frc.GetFPS());
      mFPSHistory.emplace_back(mCurrentFPS);
      timeElapsed = 0.f;
      ++mContainerStartIdx;
    }
    timeElapsed += frc.GetDeltaTime();
  }

} // namespace GUI
