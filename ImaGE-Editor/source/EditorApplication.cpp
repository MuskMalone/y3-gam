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
  ImGui_ImplGlfw_InitForOpenGL(GetWindowPointer().get(), true);
  ImGui_ImplOpenGL3_Init("#version 460 core");
}

EditorApplication::~EditorApplication() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  GetWindowPointer().reset();  // release the GLFWwindow before we terminate
  glfwTerminate();
}

void EditorApplication::Init() {
  IGE::Physics::PhysicsSystem::InitAllocator();
  IGE::Physics::PhysicsSystem::GetInstance()->Init();
  GetScene()->Init();
  Scenes::SceneManager::GetInstance().Init();
  Prefabs::PrefabManager::GetInstance().Init();
  FrameRateController::GetInstance().Init(120.f, 1.f, false);
  Input::InputManager::GetInstance().InitInputManager(GetWindowPointer(),
    GetApplicationSpecification().WindowWidth, GetApplicationSpecification().WindowHeight, 0.3);
  mGUIManager.Init();
}

void EditorApplication::Run() {
  static auto& eventManager{ Events::EventManager::GetInstance() };
  static auto& inputManager{ Input::InputManager::GetInstance() };

  while (!glfwWindowShouldClose(GetWindowPointer().get())) {
    FrameRateController::GetInstance().Start();
    try {
      if (GetApplicationSpecification().EnableImGui) {
        ImGuiStartFrame();
      }

      try {
        inputManager.UpdateInput();

        // dispatch all events in the queue at the start of game loop
        eventManager.DispatchAll();

        GetScene()->Update(FrameRateController::GetInstance().GetDeltaTime());
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

          auto fb = Graphics::Renderer::GetFinalFramebuffer();
          mGUIManager.UpdateGUI(fb);

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
      glfwSwapBuffers(GetWindowPointer().get());

      FrameRateController::GetInstance().End();
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

void EditorApplication::SetCallbacks() {
  glfwSetFramebufferSizeCallback(GetWindowPointer().get(), FramebufferSizeCallback);
  glfwSetErrorCallback(ErrorCallback);
  glfwSetDropCallback(GetWindowPointer().get(), WindowDropCallback);
}

void EditorApplication::ImGuiStartFrame() const {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::DockSpaceOverViewport();
}

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
void EditorApplication::WindowDropCallback(GLFWwindow*, int pathCount, const char* paths[]) {
  QUEUE_EVENT(Events::AddFilesFromExplorerEvent, pathCount, paths);
}

/*!*********************************************************************
\brief
  Wrapper function to print out exceptions.

\param e
  Exception caught
************************************************************************/
void EditorApplication::PrintException(Debug::ExceptionBase& e) {
  e.LogSource();
}

/*!*********************************************************************
\brief
  Wrapper function to print out exceptions.

\param e
  Exception caught
************************************************************************/
void EditorApplication::PrintException(std::exception& e) {
  if (Application::IsImGUIActive()) {
    Debug::DebugLogger::GetInstance().LogCritical(e.what());
    Debug::DebugLogger::GetInstance().PrintToCout(e.what(), Debug::LVL_CRITICAL);
  }
}