#include <pch.h>
#include "Application.h"

#include <Events/EventManager.h>
#include <Scenes/SceneManager.h>
#include <Prefabs/PrefabManager.h>
#include <Input/InputManager.h>

#include <Core/Entity.h>
#include <Core/EntityManager.h>
#include <Core/Components/Components.h>

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
  Input::InputManager::GetInstance().InitInputManager(mWindow, mWidth, mHeight, 0.1);

#ifndef IMGUI_DISABLE
  mGUIManager.Init();
#endif

  // @TODO: REMOVE, FOR TESTING ONLY
  /*
  ECS::Entity one = ECS::EntityManager::GetInstance().CreateEntityWithTag("one");
  ECS::Entity two = ECS::EntityManager::GetInstance().CreateEntityWithTag("two");
  ECS::Entity three = ECS::EntityManager::GetInstance().CreateEntityWithTag("three");
  ECS::Entity four = ECS::EntityManager::GetInstance().CreateEntityWithTag("four");
  ECS::Entity five = ECS::EntityManager::GetInstance().CreateEntityWithTag("five");
  ECS::Entity six = ECS::EntityManager::GetInstance().CreateEntityWithTag("six");
  ECS::Entity seven = ECS::EntityManager::GetInstance().CreateEntityWithTag("seven");

  ECS::EntityManager::GetInstance().SetChildEntity(one, two);
  ECS::EntityManager::GetInstance().SetChildEntity(one, three);
  ECS::EntityManager::GetInstance().SetChildEntity(two, four);
  ECS::EntityManager::GetInstance().SetChildEntity(two, five);
  ECS::EntityManager::GetInstance().RemoveParent(four);

  std::vector<ECS::Entity> list =
    ECS::EntityManager::GetInstance().GetChildEntity(two);

  std::cout << "First List: ";
  for (const auto& element : list) {
    std::cout << element.GetTag() << " ";
  }

  std::vector<ECS::Entity> listTwo =
    ECS::EntityManager::GetInstance().GetChildEntity(one);

  std::cout << "Second List: ";
  for (const auto& element : listTwo) {
    std::cout << element.GetTag() << " ";
  }

  FrameRateController::GetInstance().StartSystemTimer();

  FrameRateController::GetInstance().EndSystemTimer("Cool System");

  auto const& map{ FrameRateController::GetInstance().GetSystemTimerMap() };
  for (auto const& elem : map) {
    std::cout << elem.first << " : " << elem.second << "\n";
  }
  */

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

        mScene->Update(FrameRateController::GetInstance().GetDeltaTime());
      }
      catch (Debug::ExceptionBase& e) {
        PrintException(e);
      }
      catch (std::exception& e) {
        PrintException(e);
      }


#ifndef IMGUI_DISABLE
      try
      {
        if (mImGuiActive)
        {
          UpdateFramebuffers();

          // Update ImGui
          mGUIManager.UpdateGUI(mFramebuffers.front().first);

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
#else
      glBindFramebuffer(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT);
      mFramebuffers.front().second();
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
      }
      catch (Debug::ExceptionBase& e) {
        PrintException(e);
      }
      catch (std::exception& e) {
        PrintException(e);
      }
      // check and call events, swap buffers
      glfwSwapBuffers(mWindow.get());

      FrameRateController::GetInstance().End();
    }
    catch (Debug::ExceptionBase& e) {
      PrintException(e);
    }
    catch (std::exception& e) {
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
    throw Debug::Exception<Application>(Debug::LVL_CRITICAL, Msg("Unable to create window for application"));
  }

  glfwMakeContextCurrent(mWindow.get());
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    throw Debug::Exception<Application>(Debug::LVL_CRITICAL, Msg("Failed to initialize GLAD"));
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
#endif

  glfwSetWindowUserPointer(mWindow.get(), this); // set the window to reference this class
  
  glViewport(0, 0, width, height); // specify size of viewport
  SetCallbacks();

  mScene = std::make_unique<Scene>("./Assets/Shaders/BlinnPhong.vert.glsl", "./Assets/Shaders/BlinnPhong.frag.glsl");
  // attach each draw function to its framebuffer
  mFramebuffers.emplace_back(std::make_shared<Graphics::Framebuffer>(width, height), std::bind(&Scene::Draw, mScene.get()));
}

void Application::UpdateFramebuffers()
{
  // iterate through all framebuffers and invoke the
  // draw function associated with it
  for (auto const& [fb, drawFn] : mFramebuffers)
  {
    fb->Bind();

    drawFn();

    fb->Unbind();
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
  for (auto&[fb, fn] : app->mFramebuffers) {
    fb->Resize(width, height);
  }
}

void Application::ErrorCallback(int err, const char* desc)
{
  UNREFERENCED_PARAMETER(err);
  std::ostringstream oss{};
  oss << "GLFW ERROR: \"" << desc << "\"" << " | Error code: ";
  Debug::DebugLogger::GetInstance().LogError(oss.str());
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
  Prefabs::PrefabManager::GetInstance().Shutdown();
  Debug::DebugLogger::GetInstance().Shutdown();
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
  void PrintException(Debug::ExceptionBase& e) {
    e.LogSource();
  }

  void PrintException(std::exception& e) {
    Debug::DebugLogger::GetInstance().LogCritical(e.what());
    Debug::DebugLogger::GetInstance().PrintToCout(e.what(), Debug::LVL_CRITICAL);
  }
}