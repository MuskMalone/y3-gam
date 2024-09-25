#include <pch.h>
#include "Application.h"

#include <Events/EventManager.h>
#include <Scenes/SceneManager.h>
#include <Prefabs/PrefabManager.h>
#include <Input/InputManager.h>

#include <Core/Entity.h>
#include <Core/EntityManager.h>
#include <Core/Component/Components.h>

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

Application::ApplicationSpecification Application::mSpecification{};

void Application::Init() {
  IGE::Physics::PhysicsSystem::InitAllocator();
  IGE::Physics::PhysicsSystem::GetInstance()->Init();
  mScene->Init();
  Scenes::SceneManager::GetInstance().Init();
  Prefabs::PrefabManager::GetInstance().Init();
  FrameRateController::GetInstance().Init(120.f, 1.f, false);
  Input::InputManager::GetInstance().InitInputManager(mWindow, mSpecification.WindowWidth, mSpecification.WindowHeight, 0.3);
}

void Application::Run() {
  static auto& eventManager{ Events::EventManager::GetInstance() };
  static auto& inputManager{ Input::InputManager::GetInstance() };

  while (!glfwWindowShouldClose(mWindow.get())) {
    FrameRateController::GetInstance().Start();
    try {
      try {
        inputManager.UpdateInput();

        // dispatch all events in the queue at the start of game loop
        eventManager.DispatchAll();

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

      try {
        glBindFramebuffer(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT);
        mFramebuffers.front().second();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

Application::Application(ApplicationSpecification spec) :
  mScene{}, mWindow{}
{
  mSpecification = spec;
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
    WindowPtr temp{ glfwCreateWindow(spec.WindowWidth, spec.WindowHeight, spec.Name.c_str(), NULL, NULL) };
    mWindow = std::move(temp);
  }

  if (!mWindow) {
    throw std::runtime_error("Unable to create window for application");
  }

  glfwMakeContextCurrent(mWindow.get());
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    throw std::runtime_error("Failed to initialize GLAD");
  }

  glfwSetWindowUserPointer(mWindow.get(), this); // set the window to reference this class
  
  glViewport(0, 0, spec.WindowWidth, spec.WindowHeight); // specify size of viewport
  SetCallbacks();

  mScene = std::make_unique<Scene>("./assets/Shaders/BlinnPhong.vert.glsl", "./assets/Shaders/BlinnPhong.frag.glsl");
  // attach each draw function to its framebuffer
  mFramebuffers.emplace_back(std::piecewise_construct, std::forward_as_tuple(spec.WindowWidth, spec.WindowHeight),
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

void Application::SetCallbacks() {
  glfwSetFramebufferSizeCallback(mWindow.get(), FramebufferSizeCallback);
  glfwSetErrorCallback(ErrorCallback);
}

void Application::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);

  Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
  for (auto& elem : app->mFramebuffers) {
    elem.first.Resize(width, height);
  }
}

void Application::ErrorCallback(int err, const char* desc) {
  UNREFERENCED_PARAMETER(err);
#ifdef _DEBUG
  std::cerr << "GLFW ERROR: \"" << desc << "\"" << " | Error code: " << std::endl;
#endif
}

void Application::PrintException(Debug::ExceptionBase& e) {
  e.LogSource();
}

void Application::PrintException(std::exception& e) {
  Debug::DebugLogger::GetInstance().LogCritical(e.what());
  Debug::DebugLogger::GetInstance().PrintToCout(e.what(), Debug::LVL_CRITICAL);
}

void Application::Shutdown()
{
  Scenes::SceneManager::GetInstance().Shutdown();
}

Application::~Application()
{
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
    if (Application::IsImGUIActive()) {
      Debug::DebugLogger::GetInstance().LogCritical(e.what());
      Debug::DebugLogger::GetInstance().PrintToCout(e.what(), Debug::LVL_CRITICAL);
    }

  }
}