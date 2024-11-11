/*!*********************************************************************
\file   EditorApplication.h
\date   5-October-2024
\brief  The main class running the editor. Inherits from Application
        and performs additional initializations and updates for ImGui
        elements.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include "Application.h"
#include "GUI/GUIManager.h"
#include "Graphics/Texture.h"
#include <Events/EventCallback.h>

namespace IGE {
  class EditorApplication : public Application {
  public:

    EditorApplication(EditorApplication::ApplicationSpecification const& spec);
    ~EditorApplication();

    /*!*********************************************************************
    \brief
      Initializes the application
    ************************************************************************/
    void Init() override;

    /*!*********************************************************************
    \brief
      Update function of the application
    ************************************************************************/
    void Run() override;

    /*!*********************************************************************
    \brief
      Shuts down the application and releases its resources
    ************************************************************************/
    void Shutdown() override;

  private:
    GUI::GUIManager mGUIManager;

  private:
    /*!*********************************************************************
    \brief
      Starts the ImGui frame
    ************************************************************************/
    void ImGuiStartFrame() const;

    /*!*********************************************************************
    \brief
      Updates all framebuffers
    ************************************************************************/
    void UpdateFramebuffers(std::shared_ptr<Graphics::Texture>& tex);

    /*!*********************************************************************
    \brief
      Sets editor-specific callbacks
    ************************************************************************/
    void SetEditorCallbacks();

    /*!*********************************************************************
    \brief
      Wrapper function to print out exceptions.
    \param e
      Exception caught
    ************************************************************************/
    void PrintException(Debug::ExceptionBase& e);

    /*!*********************************************************************
    \brief
      Wrapper function to print out exceptions.
    \param e
      Exception caught
  ************************************************************************/
    void PrintException(std::exception& e);

    EVENT_CALLBACK_DECL(SignalCallback);

    /*!*********************************************************************
    \brief
      Callback function for dragging files into the editor. Sends the
      filepaths received to the asset manager.
    \param window
      The window the file was dragged into
    \param pathCount
      The number of files
    \param paths
      The paths of the files
    ************************************************************************/
    static void WindowDropCallback(GLFWwindow*, int pathCount, const char* paths[]);
  };
} // namespace IGE
