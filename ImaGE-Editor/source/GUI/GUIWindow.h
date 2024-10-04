/*!*********************************************************************
\file   GUIWindow.h
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Base class of all GUI windows in the editor. Contains the
        necessary functions and members needed for each element
        managed by the GUIManager.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#ifndef IMGUI_DISABLE
#include <string>

namespace GUI
{
  // class that defines common interface every GUI window should have
  class GUIWindow
  {
  public:
    virtual ~GUIWindow() {}

    /*!*********************************************************************
    \brief
      Checks if the window is active
    \return
      True if the window is active and false otherwise
    ************************************************************************/
    inline bool IsActive() const noexcept { return mActive; }

    /*!*********************************************************************
    \brief
      Gets the name of the window
    \return
      The name of the window
    ************************************************************************/
    inline std::string const& GetName() const noexcept { return mWindowName; }

    //virtual void Init() {}

    /*!*********************************************************************
    \brief
      Update function of the window
    ************************************************************************/
    virtual void Run() = 0;

    /*!*********************************************************************
    \brief
      Toggles the active flag of the window
    ************************************************************************/
    virtual void Toggle() { mActive = !mActive; }

  protected:
    GUIWindow(std::string windowName) : mWindowName{ std::move(windowName) }, mActive{ true } {}

    std::string const mWindowName;
    bool mActive;
  };

};  // namespace GUI

#endif  // IMGUI_DISABLE
