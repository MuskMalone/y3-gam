#include <pch.h>
#include "Application.h"

#include <Events/EventManager.h>
#include <Scenes/SceneManager.h>
#include <Prefabs/PrefabManager.h>
#include <Input/InputManager.h>

#include <Core/Entity.h>
#include <Core/EntityManager.h>
#include <Core/Component/Components.h>

#ifndef IMGUI_DISABLE
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#endif

#include <Physics/PhysicsSystem.h>

namespace
{
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

}

void Application::Init() {
  IGE::Physics::PhysicsSystem::InitAllocator();
  IGE::Physics::PhysicsSystem::GetInstance()->Init();
  mScene->Init();
  Scenes::SceneManager::GetInstance().Init();
  Prefabs::PrefabManager::GetInstance().Init();

  // @TODO: SETTINGS TO BE LOADED FROM CONFIG FILE
  FrameRateController::GetInstance().Init(120.f, 1.f, false);
  Input::InputManager::GetInstance().InitInputManager(mWindow, mWidth, mHeight, 0.3);

#ifndef IMGUI_DISABLE
  mGUIManager.Init(mFramebuffers.front().first);
#endif
}

void Application::Run() {
  static auto& eventManager{ Events::EventManager::GetInstance() };
  static auto& inputManager{ Input::InputManager::GetInstance() };

  while (!glfwWindowShouldClose(mWindow.get())) {
    FrameRateController::GetInstance().Start();
    try {

#ifndef IMGUI_DISABLE
      if (mImGuiActive) {
        ImGuiStartFrame();
      }
#endif
      try {
        inputManager.UpdateInput();

        // dispatch all events in the queue at the start of game loop
        eventManager.DispatchAll();

#ifndef IMGUI_DISABLE
        if (mImGuiActive) {
          mGUIManager.UpdateGUI();
        }
#endif

        mScene->Update(FrameRateController::GetInstance().GetDeltaTime());
      }
      catch (Debug::ExceptionBase& e)
      {
        PrintException(e);
      }
      catch (std::exception& e)
      {
        PrintException(e);
      }


#ifndef IMGUI_DISABLE
      try {
        if (mImGuiActive) {
          UpdateFramebuffers();

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
#else
      try {
        glBindFramebuffer(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT);
        mFramebuffers.front().second();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
      }
#endif
      
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

Application::Application(const char* name, int width, int height) :
#ifndef IMGUI_DISABLE
  mGUIManager{},
#endif
  mScene{}, mWindow{},
  mWidth{ width }, mHeight{ height }, mImGuiActive{ true }
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_RED_BITS, 8); glfwWindowHint(GLFW_GREEN_BITS, 8);
  glfwWindowHint(GLFW_BLUE_BITS, 8); glfwWindowHint(GLFW_ALPHA_BITS, 8);

  // initialize window ptr
  // have to do this because i can't make_unique with custom dtor
  {
    WindowPtr temp{ glfwCreateWindow(width, height, name, NULL, NULL) };
    mWindow = std::move(temp);
  }

  if (!mWindow) {
    throw std::runtime_error("Unable to create window for application");
  }

  glfwMakeContextCurrent(mWindow.get());
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    throw std::runtime_error("Failed to initialize GLAD");
  }

#ifndef IMGUI_DISABLE
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // floating windows

  ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    //style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 0.65f;
  }

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(mWindow.get(), true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
  ImGui_ImplOpenGL3_Init("#version 460 core");

  mGUIManager.StyleGUI();
#endif

  glfwSetWindowUserPointer(mWindow.get(), this); // set the window to reference this class
  
  glViewport(0, 0, width, height); // specify size of viewport
  SetCallbacks();

  mScene = std::make_unique<Scene>("./Assets/Shaders/BlinnPhong.vert.glsl", "./Assets/Shaders/BlinnPhong.frag.glsl");
  // attach each draw function to its framebuffer
  mFramebuffers.emplace_back(std::piecewise_construct, std::forward_as_tuple(width, height),
    std::forward_as_tuple(std::bind(&Scene::Draw, mScene.get())));
}

void Application::UpdateFramebuffers()
{
  // iterate through all framebuffers and invoke the
  // draw function associated with it
  for (auto const& [fb, drawFn] : mFramebuffers)
  {
    fb.Bind();

    drawFn();

    fb.Unbind();
  }
}

void Application::SetCallbacks()
{
  glfwSetFramebufferSizeCallback(mWindow.get(), FramebufferSizeCallback);
  glfwSetErrorCallback(ErrorCallback);

#ifndef IMGUI_DISABLE
  glfwSetDropCallback(mWindow.get(), WindowDropCallback);           // file drag n drop callback
#endif
}

void Application::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);

  Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
  for (auto& elem : app->mFramebuffers)
  {
    elem.first.Resize(width, height);
  }
}

void Application::ErrorCallback(int err, const char* desc)
{
  UNREFERENCED_PARAMETER(err);
#ifdef _DEBUG
  std::cerr << "GLFW ERROR: \"" << desc << "\"" << " | Error code: " << std::endl;
#endif
}

#ifndef IMGUI_DISABLE
void Application::ImGuiStartFrame() const
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::DockSpaceOverViewport(); // convert the window into a dockspace
}

void Application::WindowDropCallback(GLFWwindow*, int pathCount, const char* paths[]) {
  QUEUE_EVENT(Events::AddFilesFromExplorerEvent, pathCount, paths);
}
#endif

void Application::Shutdown()
{
  Scenes::SceneManager::GetInstance().Shutdown();
}

Application::~Application()
{
#ifndef IMGUI_DISABLE
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
#endif

  mWindow.reset();  // release the GLFWwindow before we terminate
  glfwTerminate();
}


namespace {
  void PrintException(Debug::ExceptionBase& e)
  {
    e.LogSource();
  }

  void PrintException(std::exception& e)
  {
#ifndef IMGUI_DISABLE
    Debug::DebugLogger::GetInstance().LogCritical(e.what());
    Debug::DebugLogger::GetInstance().PrintToCout(e.what(), Debug::LVL_CRITICAL);
#endif
  }
}