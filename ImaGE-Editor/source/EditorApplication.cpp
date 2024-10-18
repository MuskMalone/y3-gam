/*!*********************************************************************
\file   EditorApplication.cpp
\date   5-October-2024
\brief  The main class running the editor. Inherits from Application
        and performs additional initializations and updates for ImGui
        elements.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "EditorApplication.h"

#include <Events/EventManager.h>
#include <Physics/PhysicsSystem.h>
#include <Scenes/SceneManager.h>
#include <Prefabs/PrefabManager.h>
#include <Input/InputManager.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <Core/Systems/Systems.h>

namespace IGE {
  EditorApplication::EditorApplication(Application::ApplicationSpecification const& spec) :
    mGUIManager{}, Application(spec) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // floating windows

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      style.Colors[ImGuiCol_WindowBg].w = 0.65f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(mWindow.get(), true);
    ImGui_ImplOpenGL3_Init("#version 460 core");

    SetEditorCallbacks();
  }

  EditorApplication::~EditorApplication() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  // DO NOT PERFORM ANY ImaGE-Core INITS HERE; DO IT IN Application::Init
  void EditorApplication::Init() {
    Application::Init();  // perform default Init

    // init editor-specific stuff
    mGUIManager.Init();
  }

  void EditorApplication::Run() {
    static auto& eventManager{ Events::EventManager::GetInstance() };
    static auto& inputManager{ Input::InputManager::GetInstance() };
    static auto& frameRateController{ Performance::FrameRateController::GetInstance() };
    static auto& sceneManager{ Scenes::SceneManager::GetInstance() };

    while (!glfwWindowShouldClose(mWindow.get())) {
      frameRateController.Start();
      try {
        if (GetApplicationSpecification().EnableImGui) {
          ImGuiStartFrame();
        }

        try {
          inputManager.UpdateInput();

          // dispatch all events in the queue at the start of game loop
          eventManager.DispatchAll();

          if (sceneManager.GetSceneState() == Scenes::PLAYING) {
            mSystemManager.UpdateSystems();
          }
          else {
            mSystemManager.UpdateSelectedSystems<Systems::TransformSystem>();
          }
        }
        catch (Debug::ExceptionBase& e)
        {
          PrintException(e);
        }
        catch (std::exception& e)
        {
          PrintException(e);
        }

        try {
          if (GetApplicationSpecification().EnableImGui) {

            UpdateFramebuffers();
            
            // @TODO: is this line still needed? - u can alr directly update the framebuffer in the draw function
            mRenderTargets.front().framebuffer = Graphics::Renderer::GetFinalFramebuffer();
            mGUIManager.UpdateGUI(mRenderTargets.front());

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // for floating windows feature
            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
              GLFWwindow* backup_current_context = glfwGetCurrentContext();
              ImGui::UpdatePlatformWindows();
              ImGui::RenderPlatformWindowsDefault();
              glfwMakeContextCurrent(backup_current_context);
            }
          }
        }

        catch (Debug::ExceptionBase& e)
        {
          PrintException(e);
        }
        catch (std::exception& e)
        {
          PrintException(e);
        }

        // check and call events, swap buffers
        glfwSwapBuffers(mWindow.get());

        frameRateController.End();
      }
      catch (Debug::ExceptionBase& e)
      {
        PrintException(e);
      }
      catch (std::exception& e)
      {
        PrintException(e);
      }
    }
  }

  void EditorApplication::UpdateFramebuffers() {
    // iterate through all render targets and
    // draw each scene to its framebuffer
    for (Graphics::RenderTarget const& target : mRenderTargets)
    {
      target.framebuffer->Bind();

      target.scene.Draw();

      target.framebuffer->Unbind();
    }
  }

  void EditorApplication::SetEditorCallbacks() {
    glfwSetDropCallback(mWindow.get(), WindowDropCallback);
  }

  void EditorApplication::ImGuiStartFrame() const {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport();
  }

  void EditorApplication::Shutdown()
  {
    // shutdown editor-specific stuff
    mGUIManager.Shutdown();

    // perform default shutdown
    Application::Shutdown();
  }

  void EditorApplication::WindowDropCallback(GLFWwindow*, int pathCount, const char* paths[]) {
    QUEUE_EVENT(Events::AddFilesFromExplorerEvent, pathCount, paths);
  }

  void EditorApplication::PrintException(Debug::ExceptionBase& e) {
    e.LogSource();
  }

  void EditorApplication::PrintException(std::exception& e) {
    if (Application::IsImGUIActive()) {
      Debug::DebugLogger::GetInstance().LogCritical(e.what());
      Debug::DebugLogger::GetInstance().PrintToCout(e.what(), Debug::LVL_CRITICAL);
    }
  }
} // namespace IGE
