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
#include <Graphics/MaterialTable.h>
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

    SUBSCRIBE_CLASS_FUNC(Events::EventType::SIGNAL, &EditorApplication::SignalCallback, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::LOCK_MOUSE, &EditorApplication::LockMouse, this);
  }

  void EditorApplication::Run() {
    static auto& eventManager{ Events::EventManager::GetInstance() };
    static auto& inputManager{ Input::InputManager::GetInstance() };
    static auto& frameRateController{ Performance::FrameRateController::GetInstance() };
    static auto& sceneManager{ Scenes::SceneManager::GetInstance() };
    static auto& sysManager{ Systems::SystemManager::GetInstance() };

    while (!glfwWindowShouldClose(mWindow.get())) {
        if (inputManager.IsKeyTriggered(IK_K)) {
            ToggleImGuiEnabled();
        }


      frameRateController.Start();
      try {
        if (GetApplicationSpecification().EnableImGui) {
          ImGuiStartFrame();
        }

        try {
          frameRateController.StartSystemTimer();
          inputManager.UpdateInput();
          frameRateController.EndSystemTimer("Input Manager");

          // dispatch all events in the queue at the start of game loop
          frameRateController.StartSystemTimer();
          eventManager.DispatchAll();
          frameRateController.EndSystemTimer("Event Manager");
          
          if (sceneManager.GetSceneState() == Scenes::PLAYING) {
            sysManager.UpdateSystems();
          }
          else {
            sysManager.PausedUpdate<Systems::TransformSystem, IGE::Physics::PhysicsSystem, IGE::Audio::AudioSystem>();
            sceneManager.ExecuteMainThreadQueue();
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

            frameRateController.StartSystemTimer();
            std::shared_ptr<Graphics::Texture> gameTex{ nullptr }; //texture to copy for game view

            UpdateFramebuffers(gameTex);
            
            auto& fb{ GetDefaultRenderTarget().framebuffer };
            fb = Graphics::Renderer::GetFinalFramebuffer();
            mGUIManager.UpdateGUI(fb, gameTex);
            frameRateController.EndSystemTimer("Graphics System");
            
            frameRateController.StartSystemTimer();
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

            frameRateController.EndSystemTimer("ImGui");
          }
          else {
              glBindFramebuffer(GL_FRAMEBUFFER, 0);
              int width, height;
              glfwGetFramebufferSize(mWindow.get(), &width, &height);

              // Set the viewport
              glViewport(0, 0, width, height);

              // Clear the screen
              glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

              if (Graphics::RenderSystem::mCameraManager.HasActiveCamera()) {
                  Graphics::RenderSystem::RenderScene(Graphics::CameraSpec{ Graphics::RenderSystem::mCameraManager.GetActiveCameraComponent() });
              }
              auto const& fb = Graphics::Renderer::GetFinalFramebuffer();
              std::shared_ptr<Graphics::Texture> gameTex = std::make_shared<Graphics::Texture>(fb->GetFramebufferSpec().width, fb->GetFramebufferSpec().height);

              if (gameTex) {
                  gameTex->CopyFrom(fb->GetColorAttachmentID(), fb->GetFramebufferSpec().width, fb->GetFramebufferSpec().height);
              }

              gameTex->Bind(0);
              auto const& shader = Graphics::ShaderLibrary::Get("FullscreenQuad");
              shader->Use();
              Graphics::Renderer::RenderFullscreenTexture();
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
      IGE_EVENTMGR.DispatchImmediateEvent<Events::SignalEvent>();   // workaround to calling EditorApplication member function
      std::cerr << ">>>>>>>>>>>>>>>>>>>>>> Crash detected! Scene and Metadata has been backed-up to \"./.backup/\" folder <<<<<<<<<<<<<<<<<<<\n";
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

  // ensure proper shutdown in case of crash
  EVENT_CALLBACK_DEF(EditorApplication, SignalCallback) {
    IGE_SCENEMGR.BackupSave();
    IGE_ASSETMGR.SaveMetadata();

    EditorApplication::Shutdown();
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

  EVENT_CALLBACK_DEF(EditorApplication, LockMouse) {
    if (CAST_TO_EVENT(Events::LockMouseEvent)->isLocked)
    {
      // Set the cursor position to the center of the window
      glfwSetCursorPos(mWindow.get(), mSpecification.WindowWidth / 2.0, mSpecification.WindowHeight / 2.0);
      glfwSetInputMode(mWindow.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    else
      glfwSetInputMode(mWindow.get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    //CAST_TO_EVENT(Events::LockMouseEvent)->isLocked = !(CAST_TO_EVENT(Events::LockMouseEvent)->isLocked);
  }

} // namespace IGE
