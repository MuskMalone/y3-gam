/*!*********************************************************************
\file   PerformanceWindow.h
\author chengen.lau\@digipen.edu
\date   6-October-2024
\brief  Class encapsulating functions to run the performance window
        editor. Displays information related to the engine's status
        and performance such as FPS, system update times and memory.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <GUI/GUIWindow.h>
#include <vector>

namespace GUI {
  class PerformanceWindow : public GUIWindow
  {
  public:
    PerformanceWindow(const char* name);

    /*!*********************************************************************
    \brief
      Update function of the performance window
    ************************************************************************/
    void Run() override;

  private:
    /*!*********************************************************************
    \brief
      Updates the FPS tab based on data from the frame rate controller
    ************************************************************************/
    void UpdateFPS();

    void UpdateSystemPerformance();


    static constexpr unsigned sResizeThreshold = 300; // optimization; size to hit before removing elements from the vector

    std::vector<float> mFPSHistory;
    unsigned mContainerStartIdx;  // curr idx denoting where to start reading data from
    float mTimePerUpdate, mCurrentFPS;
    int mFPSMaxCount;

    std::map<std::string, std::vector<float>> mSystemUpdateHistory;
    std::map<std::string, int> mSystemContainerStartIdx;
    std::map<std::string, float> mSystemPercentages;
  };
} // namespace GUI
