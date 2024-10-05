/*!*********************************************************************
\file   SceneControls.h
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Class encapsulating functions to run the secondary menu bar
        of the editor. Calls upon the SceneManager to start/pause/stop
        the current scene using the controls displayed.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#ifndef IMGUI_DISABLE
#include <GUI/GUIWindow.h>
#include <Scenes/SceneManager.h>
#include "GUI/GUIManager.h"

namespace GUI
{

  class SceneControls : public GUIWindow
  {
  public:
    SceneControls(const char* name);

    /*!*********************************************************************
    \brief
      Runs the scene controls menu bar
    ************************************************************************/
    void Run() override;

  private:
    Scenes::SceneManager& mSceneManager;
  };

} // namespace GUI

#endif  // IMGUI_DISABLE
