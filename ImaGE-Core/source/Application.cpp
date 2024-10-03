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

// Static Initialization
Application::ApplicationSpecification Application::mSpecification{};

void Application::Init() {
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
    inputManager.UpdateInput();

    // dispatch all events in the queue at the start of game loop
    eventManager.DispatchAll();

    mScene->Update(FrameRateController::GetInstance().GetDeltaTime());

    glBindFramebuffer(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT);
    mFramebuffers.front().second();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // check and call events, swap buffers
    glfwSwapBuffers(mWindow.get());

    FrameRateController::GetInstance().End();
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

  mScene = std::make_unique<Scene>("../Assets/Shaders/BlinnPhong.vert.glsl", "../Assets/Shaders/BlinnPhong.frag.glsl");
  // attach each draw function to its framebuffer
  // 
  //framebuffer init
  Graphics::FramebufferSpec framebufferSpec;
  framebufferSpec.width = spec.WindowWidth;
  framebufferSpec.height = spec.WindowHeight;
  framebufferSpec.attachments = { Graphics::FramebufferTextureFormat::RGBA8, Graphics::FramebufferTextureFormat::DEPTH };

  mFramebuffers.emplace_back(Graphics::Framebuffer::Create(framebufferSpec), std::bind(&Scene::Draw, mScene.get()));

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

void Application::SetCallbacks() {
  glfwSetFramebufferSizeCallback(mWindow.get(), FramebufferSizeCallback);
  glfwSetErrorCallback(ErrorCallback);
}

void Application::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);

  Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
  for (auto& [fb, fn] : app->mFramebuffers) {
    fb->Resize(width, height);
  }
}

void Application::ErrorCallback(int err, const char* desc) {
  UNREFERENCED_PARAMETER(err);
#ifdef _DEBUG
  std::cerr << "GLFW ERROR: \"" << desc << "\"" << " | Error code: " << std::endl;
#endif
}

void Application::Shutdown()
{
  Scenes::SceneManager::GetInstance().Shutdown();
  Debug::DebugLogger::GetInstance().Shutdown();
}

Application::~Application()
{
  mWindow.reset();  // release the GLFWwindow before we terminate
  glfwTerminate();
}