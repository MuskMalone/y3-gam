/*!*********************************************************************
\file   PerformanceWindow.cpp
\author chengen.lau\@digipen.edu
\date   6-October-2024
\brief  Class encapsulating functions to run the performance window
        editor. Displays information related to the engine's status
        and performance such as FPS, system update times and memory.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "PerformanceWindow.h"
#include <ImGui/imgui.h>
#include <numeric>
#include "GUI/GUIVault.h"
#include <Graphics/RenderSystem.h>

namespace GUI {

  PerformanceWindow::PerformanceWindow(const char* name) : GUIWindow(name),
    mFPSHistory{}, mContainerStartIdx{}, mTimePerUpdate{ 0.05f }, mCurrentFPS{}, mFPSMaxCount{ 120 } {
    mFPSHistory.reserve(sResizeThreshold);
  }

  void PerformanceWindow::Run() {
    ImGui::Begin(mWindowName.c_str());
    UpdateFPS();
    UpdateSystemPerformance();

    float const sum{ std::accumulate(mFPSHistory.begin(), mFPSHistory.end(), 0.f) };
    std::string const averageStr{ "Average: " + std::to_string(sum / static_cast<float>(mFPSHistory.size())) };
    ImVec2 chartSize{ ImGui::GetContentRegionAvail() };
    chartSize.y /= 4; // take up half the height

    ImGui::PushFont(GUI::GUIVault::GetStyler().GetCustomFont(GUI::MONTSERRAT_SEMIBOLD));
    ImGui::Text("FPS Chart");
    ImGui::PopFont();

    ImGui::PlotLines("##FPSChart", mFPSHistory.data(), mFPSMaxCount,
      mContainerStartIdx, averageStr.c_str(), 0.f,
      static_cast<float>(Performance::FrameRateController::GetInstance().GetTargetFPS()) + 10.f, chartSize);

    ImGui::Text(("Current: " + std::to_string(mCurrentFPS)).c_str());

    {
      float const colWidth{ ImGui::GetContentRegionAvail().x * 0.5f };
      if (ImGui::BeginTable("SettingsTable", 2, ImGuiTableFlags_None)) {
        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed, colWidth);
        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthFixed);
        float const elemSize{ colWidth * 0.8f };

        ImGui::TableNextColumn(); ImGui::Text("Width");
        ImGui::TableNextColumn(); ImGui::Text("Time per Update");

        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(elemSize);
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

        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(elemSize);
        ImGui::SliderFloat("##TimePerUpdate", &mTimePerUpdate, 0.001f, 5.f);

        ImGui::EndTable();
      }
    }

    // Graphics Stats
    ImGui::NewLine();
    ImGui::PushFont(GUI::GUIVault::GetStyler().GetCustomFont(GUI::MONTSERRAT_SEMIBOLD));
    ImGui::Text("Graphics Stats");
    ImGui::PopFont();

    ImGui::Text("Objects Culled (Editor): "); ImGui::SameLine();
    ImGui::Text(std::to_string(Graphics::RenderSystem::GetEditorCullCount()).c_str());

    ImGui::Text("Objects Culled (Game): "); ImGui::SameLine();
    ImGui::Text(std::to_string(Graphics::RenderSystem::GetGameCullCount()).c_str());

    // Overall Performance
    ImGui::NewLine();
    ImGui::PushFont(GUI::GUIVault::GetStyler().GetCustomFont(GUI::MONTSERRAT_SEMIBOLD));
    ImGui::Text("Overall System Performance (In Percentage)");
    ImGui::PopFont();
    for (const auto& [systemName, systemPercentage] : mSystemPercentages) {
      std::ostringstream percentStream;
      percentStream << systemName << ": " << std::fixed << std::setprecision(2) << systemPercentage << "%";
      ImGui::Text(percentStream.str().c_str());
    }
    ImGui::NewLine();

    // System Performance Charts
    ImGui::PushFont(GUI::GUIVault::GetStyler().GetCustomFont(GUI::MONTSERRAT_SEMIBOLD));
    ImGui::Text("Individual System Charts");
    ImGui::PopFont();
 
    chartSize.y = (chartSize.y / 4) * 3;

    for (const auto& [systemName, history] : mSystemUpdateHistory) {
      float avgTimeInSeconds = std::accumulate(history.begin(), history.end(), 0.f) / history.size();
      float avgTimeInMilliseconds = avgTimeInSeconds * 1000.0f;

      std::ostringstream avgStream;
      if (avgTimeInMilliseconds > 0.001f) {
        avgStream << "Avg: " << std::fixed << std::setprecision(3) << avgTimeInMilliseconds << " ms";
      }
      else {
        avgStream << "Avg: <0.001 ms";
      }

      float systemPercentage = mSystemPercentages[systemName];
      std::ostringstream percentStream;
      percentStream << "Overall Percentage: " << std::fixed << std::setprecision(2) << systemPercentage << "%";

      ImGui::Text((systemName + " Performance").c_str());
      ImGui::PlotLines(("##" + systemName).c_str(), history.data(), static_cast<int>(history.size()),
        mSystemContainerStartIdx[systemName], avgStream.str().c_str(), 0.f, *std::max_element(history.begin(), history.end()), chartSize);

      ImGui::Text(percentStream.str().c_str());

      ImGui::NewLine();
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

  void PerformanceWindow::UpdateSystemPerformance() {
    static float timeElapsed{};
    Performance::FrameRateController& frc{ Performance::FrameRateController::GetInstance() };

    if (timeElapsed >= mTimePerUpdate) {
      float totalSystemTime = 0.0f;

      for (const auto& [systemName, systemTimeData] : frc.GetSystemTimerMap()) {
        totalSystemTime += std::chrono::duration<float>(systemTimeData).count();
      }

      for (const auto& [systemName, systemTimeData] : frc.GetSystemTimerMap()) {
        auto& history = mSystemUpdateHistory[systemName];
        auto& startIdx = mSystemContainerStartIdx[systemName];

        if (history.size() >= sResizeThreshold) {
          history = { history.begin() + startIdx, history.end() };
          history.reserve(sResizeThreshold);
          startIdx = 0;
        }

        float systemDurationInSeconds = std::chrono::duration<float>(systemTimeData).count();
        history.emplace_back(systemDurationInSeconds);
        ++startIdx;

        float systemPercentage = (systemDurationInSeconds / totalSystemTime) * 100.0f;
        mSystemPercentages[systemName] = systemPercentage;
      }

      timeElapsed = 0.f;
    }

    timeElapsed += frc.GetDeltaTime();
  }

} // namespace GUI
