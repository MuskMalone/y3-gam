/*!*********************************************************************
\file   Application.cpp
\date   5-October-2024
\brief  The main class running the engine. Updates all systems in the
        engine and sets up the GLFW window.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "Application.h"

#pragma region SingletonIncludes
#include <Events/EventManager.h>
#include <Scenes/SceneManager.h>
#include <Prefabs/PrefabManager.h>
#include <Input/InputManager.h>
#include <Scripting/ScriptManager.h>
#include <Reflection/ObjectFactory.h>
#include <Core/EntityManager.h>
#pragma endregion

#include <Core/Entity.h>
#include <Core/Components/Components.h>
#include "Asset/IGEAssets.h"

#include <Core/Systems/Systems.h>

namespace IGE {
  // Static Initialization
  Application::ApplicationSpecification Application::mSpecification{};

  void Application::Init() {
    // initialize singletons
    Debug::DebugLogger::CreateInstance();
    Events::EventManager::CreateInstance();
    Assets::AssetManager::CreateInstance();
    Reflection::ObjectFactory::CreateInstance();
    Scenes::SceneManager::CreateInstance();
    Prefabs::PrefabManager::CreateInstance();
    Performance::FrameRateController::CreateInstance(120.f, 1.f, false);
    Input::InputManager::CreateInstance(mWindow, mSpecification.WindowWidth, mSpecification.WindowHeight, 0.1);
    Mono::ScriptManager::CreateInstance();
    ECS::EntityManager::CreateInstance();

    // @TODO: Init physics and audio singletons
    //IGE::Physics::PhysicsSystem::InitAllocator();
    //IGE::Physics::PhysicsSystem::GetInstance()->Init();

    RegisterSystems();
    IGEAssetsInitialize();
    mSystemManager.InitSystems();
    GetDefaultRenderTarget().scene.Init();
  }

  void Application::Run() {
    static auto& eventManager{ Events::EventManager::GetInstance() };
    static auto& inputManager{ Input::InputManager::GetInstance() };
    static auto& frameRateController{ Performance::FrameRateController::GetInstance() };

    while (!glfwWindowShouldClose(mWindow.get())) {
      frameRateController.Start();
      inputManager.UpdateInput();

      // dispatch all events in the queue at the start of game loop
      eventManager.DispatchAll();

      mSystemManager.UpdateSystems();
      GetDefaultRenderTarget().scene.Update(frameRateController.GetDeltaTime());

      glBindFramebuffer(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT);
      GetDefaultRenderTarget().scene.Draw();
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      // check and call events, swap buffers
      glfwSwapBuffers(mWindow.get());

      frameRateController.End();
    }
  }

  // registration order is the update order
  void Application::RegisterSystems() {
    mSystemManager.RegisterSystem<Systems::TransformSystem>("Pre-Transform System"); // must be called first   
    mSystemManager.RegisterSystem<IGE::Physics::PhysicsSystem>("Physics System");
    mSystemManager.RegisterSystem<Mono::ScriptingSystem>("Scripting System");
    //mSystemManager.RegisterSystem<Graphics::RenderSystem>("Render System");

    // dont think i need this anymore
    //mSystemManager.RegisterSystem<Systems::LocalToWorldTransformSystem>("Post-Transform System");
  }

  Application::Application(ApplicationSpecification spec) :
    mSystemManager{}, mRenderTargets{}, mWindow{}
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

  // render target init
  Graphics::FramebufferSpec framebufferSpec;
  framebufferSpec.width = spec.WindowWidth;
  framebufferSpec.height = spec.WindowHeight;
  framebufferSpec.attachments = { Graphics::FramebufferTextureFormat::RGBA8, Graphics::FramebufferTextureFormat::DEPTH };
  mRenderTargets.emplace_back(framebufferSpec);
}

  void Application::SetCallbacks() {
    glfwSetFramebufferSizeCallback(mWindow.get(), FramebufferSizeCallback);
    glfwSetErrorCallback(ErrorCallback);
  }

  void Application::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    for (auto& [fb, fn] : app->mRenderTargets) {
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
    mSystemManager.Shutdown();

    // shutdown singletons
    Scenes::SceneManager::DestroyInstance();
    Prefabs::PrefabManager::DestroyInstance();
    Mono::ScriptManager::DestroyInstance();
    Reflection::ObjectFactory::DestroyInstance();
    Performance::FrameRateController::DestroyInstance();
    Events::EventManager::DestroyInstance();
    ECS::EntityManager::DestroyInstance();
    Input::InputManager::DestroyInstance();
    Assets::AssetManager::DestroyInstance();
    Debug::DebugLogger::DestroyInstance();

    // @TODO: Shutdown physics and audio singletons
  }

  Application::~Application()
  {
    mWindow.reset();  // release the GLFWwindow before we terminate
    glfwTerminate();
  }
} // namespace IGE
