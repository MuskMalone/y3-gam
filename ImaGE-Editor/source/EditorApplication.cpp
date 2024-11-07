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

#include <Core/Systems/SystemManager/SystemManager.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <Core/Systems/Systems.h>
#include "Graphics/CameraSpec.h"

#include <Graphics/Renderer.h>
#include <Graphics/RenderPass/GeomPass.h>
#include <csignal>

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
    // im not sure if this is scalable;
    // may just make SystemManager globally accesible in future
    // if more stuff requires it
    mGUIManager.Init(GetDefaultRenderTarget());
  }

  void EditorApplication::Run() {
    static auto& eventManager{ Events::EventManager::GetInstance() };
    static auto& inputManager{ Input::InputManager::GetInstance() };
    static auto& frameRateController{ Performance::FrameRateController::GetInstance() };
    static auto& sceneManager{ Scenes::SceneManager::GetInstance() };
    static auto& sysManager{ Systems::SystemManager::GetInstance() };

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
            sysManager.UpdateSystems();
          }
          else {
            sysManager.UpdateSelectedSystems<Systems::TransformSystem, IGE::Physics::PhysicsSystem, IGE::Audio::AudioSystem>();
          }
        }
        catch (Debug::ExceptionBase& e)
        {
          PrintException(e);
#ifdef _DEBUG
          std::cerr << e.ErrMsg() << std::endl;
#endif
        }
        catch (std::exception& e)
        {
          PrintException(e);
#ifdef _DEBUG
          std::cerr << e.what() << std::endl;
#endif
        }

        try {
          if (GetApplicationSpecification().EnableImGui) {

            std::shared_ptr<Graphics::Texture> gameTex{ nullptr }; //texture to copy for game view

            UpdateFramebuffers(gameTex);
            
            auto& fb{ GetDefaultRenderTarget().framebuffer };
            fb = Graphics::Renderer::GetFinalFramebuffer();
            mGUIManager.UpdateGUI(fb, gameTex);

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
#ifdef _DEBUG
          std::cerr << e.ErrMsg() << std::endl;
#endif
        }
        catch (std::exception& e)
        {
          PrintException(e);
#ifdef _DEBUG
          std::cerr << e.what() << std::endl;
#endif
        }

        // check and call events, swap buffers
        glfwSwapBuffers(mWindow.get());

        frameRateController.End();
      }
      catch (Debug::ExceptionBase& e)
      {
        PrintException(e);
#ifdef _DEBUG
        std::cerr << e.ErrMsg() << std::endl;
#endif
      }
      catch (std::exception& e)
      {
        PrintException(e);
#ifdef _DEBUG
        std::cerr << e.what() << std::endl;
#endif
      }
    }
  }

  void EditorApplication::UpdateFramebuffers(std::shared_ptr<Graphics::Texture>& gameTex) {
    // iterate through all render targets and
    // draw each scene to its framebuffer
    //for (Graphics::RenderTarget const& target : mRenderTargets)
    //{
    //  target.framebuffer->Bind();
    //  auto const& cam = target.camera;
    //  
    //  Graphics::RenderSystem::RenderScene(Graphics::CameraSpec{cam.GetViewProjMatrix(), cam.GetPosition(), cam.GetNearPlane(), cam.GetFarPlane(), cam.GetFOV()});

    //  target.framebuffer->Unbind();
    //}
      if (mGUIManager.IsGameViewActive() && Graphics::RenderSystem::mCameraManager.HasActiveCamera()) {
          Graphics::RenderSystem::RenderScene(Graphics::CameraSpec{ Graphics::RenderSystem::mCameraManager.GetActiveCameraComponent() });
          auto const& fb0 = Graphics::Renderer::GetFinalFramebuffer();
          gameTex = std::make_shared<Graphics::Texture>(fb0->GetFramebufferSpec().width, fb0->GetFramebufferSpec().height);

          if (gameTex) {
              gameTex->CopyFrom(fb0->GetColorAttachmentID(), fb0->GetFramebufferSpec().width, fb0->GetFramebufferSpec().height);
          }

      }

      Graphics::RenderTarget const& target = mRenderTargets[0];
      auto const& cam = target.camera;
      Graphics::RenderSystem::RenderScene(Graphics::CameraSpec{ cam.GetViewProjMatrix(), cam.GetViewMatrix(), cam.GetPosition(), cam.GetNearPlane(), cam.GetFarPlane(), cam.GetFOV(), cam.GetAspectRatio(), true});

  }

  void EditorApplication::SetEditorCallbacks() {
    glfwSetDropCallback(mWindow.get(), WindowDropCallback);

    auto lambd = [](int signal) { 
      std::cerr << ">>>>>>>>>>>>>>>>>>>>>> Crash detected! Scene and Metadata has been backed-up to \"./.backup/\" folder <<<<<<<<<<<<<<<<<<<\n";
      Scenes::SceneManager::GetInstance().BackupSave();
      IGE_ASSETMGR.SaveMetadata();

      IGE_DBGLOGGER.DestroyInstance();
    };

    std::signal(SIGABRT, lambd);
    std::signal(SIGILL, lambd);
    std::signal(SIGSEGV, lambd);
    std::signal(SIGFPE, lambd);
    std::signal(SIGTERM, lambd);
  }

  void EditorApplication::ImGuiStartFrame() const {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
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
