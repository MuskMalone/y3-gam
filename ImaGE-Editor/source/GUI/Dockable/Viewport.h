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
#include <EditorCamera.h>
#include <memory>
#include <Events/EventCallback.h>

namespace GUI
{

  class Viewport : public GUIWindow
  {
  public:
    Viewport(const char* name, std::shared_ptr<Graphics::EditorCamera> camera);
    
    void Run() override {}  // not in use

    /*!*********************************************************************
    \brief
      Renders the framebuffer to the ImGui window as an image
    \param renderTarget
      The render target holding the framebuffer and camera
    ************************************************************************/
    void Render(std::shared_ptr<Graphics::Framebuffer> const& framebuffer);

  private:
    static inline constexpr const char* sUnsavedChangesPopupTitle = "WARNING: Scene has been Modified!";
    static inline constexpr float sEntityScaleFactor = 1.f; // for camera zooming

    std::shared_ptr<Graphics::EditorCamera> mEditorCam;
    bool mIsPanning, mRightClickHeld, mFocusWindow;
    bool mUnsavedChangesPopup;

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

    bool UpdateViewManipulate(ImVec2 const& windowPos, ImVec2 const& size);

    /*!*********************************************************************
    \brief
      Checks the viewport for any DragDrop operations and handles them
      accordingly
    ************************************************************************/
    void ReceivePayload();

    void UnsavedChangesPopup();

    EVENT_CALLBACK_DECL(OnEntityDoubleClicked);
    EVENT_CALLBACK_DECL(OnSceneStart);
    EVENT_CALLBACK_DECL(OnCollectEditorData);
    EVENT_CALLBACK_DECL(OnLoadEditorData);
  };
  
} // namespace GUI
