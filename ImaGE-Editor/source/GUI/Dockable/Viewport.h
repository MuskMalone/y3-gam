/*!*********************************************************************
\file   Viewport.h
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Class encapsulating functions to run the editor viewport
        window of the editor. Renders the scene onto the ImGui window
        by attaching the framebuffer as an image. Also allows input
        operations such as DragDrop and selection / gizmos (in future).

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <GUI/GUIWindow.h>
#include <Graphics/RenderTarget.h>  // for framebuffer and camera
#include <memory>
#include <Events/EventCallback.h>

namespace GUI
{

  class Viewport : public GUIWindow
  {
  public:
    Viewport(const char* name, Graphics::EditorCamera& camera);
    
    void Run() override {}  // not in use

    /*!*********************************************************************
    \brief
      Renders the framebuffer to the ImGui window as an image
    \param renderTarget
      The render target holding the framebuffer and camera
    ************************************************************************/
    void Render(std::shared_ptr<Graphics::Framebuffer> const& framebuffer);

  private:
    static inline constexpr float sEntityScaleFactor = 1.f; // for camera zooming

    Graphics::EditorCamera& mEditorCam;
    bool mIsPanning, mRightClickHeld, mFocusWindow;

    /*!*********************************************************************
    \brief
      Updates the camera based on inputs to the viewport
    \param cam
      The camera to update
    ************************************************************************/
    void ProcessCameraInputs();

    /*!*********************************************************************
    \brief
      Updates ImGuizmos controls based on the selected entity
    ************************************************************************/
    bool UpdateGuizmos() const;

    /*!*********************************************************************
    \brief
      Checks the viewport for any DragDrop operations and handles them
      accordingly
    ************************************************************************/
    void ReceivePayload();

    EVENT_CALLBACK_DECL(OnEntityDoubleClicked);
    EVENT_CALLBACK_DECL(OnSceneStart);
  };
  
} // namespace GUI
