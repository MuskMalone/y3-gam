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
#include <Graphics/PostProcessing/PostProcessingManager.h>
#pragma endregion

#include <Core/Entity.h>
#include <Core/Components/Components.h>
#include "Asset/IGEAssets.h"
#include "Graphics/Renderer.h"

#pragma region SYSTEM_INCLUDES
#include <Core/Systems/SystemManager/SystemManager.h>
#include <Physics/PhysicsSystem.h>
#include <Audio/AudioManager.h>
#include <Core/Systems/TransformSystem/TransformSystem.h>
#include <Scripting/ScriptingSystem.h>
#include <Core/LayerManager/LayerManager.h>
#include <Graphics/RenderSystem.h>
#include <Core/Systems/Systems.h>
#include <Audio/AudioSystem.h>
#include <Core/LayerManager/LayerManager.h>
#pragma endregion

#include "Serialization/Serializer.h"
#include "Serialization/Deserializer.h"
#include "Asset/AssetMetadata.h"

namespace IGE {
  // Static Initialization
  Application::ApplicationSpecification Application::mSpecification{};

  void Application::Init() {
    // initialize singletons
    Debug::DebugLogger::CreateInstance();
    Events::EventManager::CreateInstance();
    IGE::Audio::AudioManager::CreateInstance();
    Assets::AssetManager::CreateInstance();
    Reflection::ObjectFactory::CreateInstance();
    Scenes::SceneManager::CreateInstance();
    Prefabs::PrefabManager::CreateInstance();
    Performance::FrameRateController::CreateInstance(120.f, 0.05f, false);
    Input::InputManager::CreateInstance(mWindow, mSpecification.WindowWidth, mSpecification.WindowHeight, 0.1);
    Mono::ScriptManager::CreateInstance();
    ECS::EntityManager::CreateInstance();
    Layers::LayerManager::CreateInstance();
    Systems::SystemManager::CreateInstance();
    Graphics::PostProcessingManager::CreateInstance();

    
    // @TODO: Init physics and audio singletons
    //IGE::Physics::PhysicsSystem::InitAllocator();
    //IGE::Physics::PhysicsSystem::GetInstance()->Init();

    RegisterSystems();
    Systems::SystemManager::GetInstance().InitSystems();
    Graphics::RenderSystem::Init();

    int width, height;
    glfwGetFramebufferSize(mWindow.get(), &width, &height);
    glViewport(0, 0, width, height);
    //Graphics::Renderer::ResizeFinalFramebuffer(width, height);
    QUEUE_EVENT(Events::WindowResized, width, height);

    
    if (mSpecification.StartFromScene.first) {
      QUEUE_EVENT(Events::LoadSceneEvent, std::filesystem::path(mSpecification.StartFromScene.second).stem().string(), 
        mSpecification.StartFromScene.second);
    }
  }

  void Application::Run() {
    static auto& eventManager{ Events::EventManager::GetInstance() };
    static auto& inputManager{ Input::InputManager::GetInstance() };
    static auto& frameRateController{ Performance::FrameRateController::GetInstance() };
    static auto& systemManager{ Systems::SystemManager::GetInstance() };

    while (!glfwWindowShouldClose(mWindow.get())) { 
      frameRateController.Start();

      if(inputManager.IsKeyTriggered(IK_F11)) //TODO Change to Event based
          Application::ToggleFullscreen();

      inputManager.UpdateInput();

      // dispatch all events in the queue at the start of game loop
      eventManager.DispatchAll();

      systemManager.UpdateSystems();

      //=======================================================================
      Graphics::RenderSystem::RenderScene(Graphics::CameraSpec{ Graphics::RenderSystem::mCameraManager.GetActiveCameraComponent() });
      auto const& fb = Graphics::Renderer::GetFinalFramebuffer();
      std::shared_ptr<Graphics::Texture> gameTex = std::make_shared<Graphics::Texture>(fb->GetFramebufferSpec().width, fb->GetFramebufferSpec().height);

      if (gameTex) {
          gameTex->CopyFrom(fb->GetColorAttachmentID(), fb->GetFramebufferSpec().width, fb->GetFramebufferSpec().height);
      }

      gameTex->Bind(0);
      auto const& shader = Graphics::ShaderLibrary::Get("FullscreenQuad");
      shader->Use();
      Graphics::Renderer::RenderFullscreenTexture();
      //===========================================================================
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      // check and call events, swap buffers
      glfwSwapBuffers(mWindow.get());

      frameRateController.End();
    }
  }

  // registration order is the update order
  void Application::RegisterSystems() {
    Systems::SystemManager& systemManager{ Systems::SystemManager::GetInstance() };

    systemManager.RegisterSystem<Systems::TransformSystem>("Transform System");
    systemManager.RegisterSystem<IGE::Physics::PhysicsSystem>("Physics System");
    systemManager.RegisterSystem<Mono::ScriptingSystem>("Scripting System");
    systemManager.RegisterSystem<IGE::Audio::AudioSystem>("Audio System");
    systemManager.RegisterSystem<Systems::TextSystem>("Text System");
  }

  Application::Application(ApplicationSpecification spec) : mRenderTargets{}, mWindow{}
  {
    mSpecification = spec;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_RED_BITS, 8); glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8); glfwWindowHint(GLFW_ALPHA_BITS, 8);

    glfwWindowHint(GLFW_RESIZABLE, mSpecification.Resizable ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_MAXIMIZED, mSpecification.StartMaximized ? GLFW_TRUE : GLFW_FALSE);

    // initialize window ptr
    // have to do this because i can't make_unique with custom dtor
    {
      WindowPtr temp{ glfwCreateWindow(spec.WindowWidth, spec.WindowHeight, spec.Name.c_str(), NULL, NULL) };
      mWindow = std::move(temp);
    }

    if (!mWindow) {
      throw std::runtime_error("Unable to create window for application");
    }

    GLFWmonitor* monitor = nullptr;
    if (mSpecification.Fullscreen) {
      glfwGetWindowPos(mWindow.get(), &mWindowState.windowedPosX, &mWindowState.windowedPosY);
      glfwGetWindowSize(mWindow.get(), &mWindowState.windowedWidth, &mWindowState.windowedHeight);

      monitor = glfwGetPrimaryMonitor();
      const GLFWvidmode* mode = glfwGetVideoMode(monitor);
      glfwSetWindowMonitor(mWindow.get(), monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
      mWindowState.isFullscreen = true;
    }

    glfwMakeContextCurrent(mWindow.get());
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      throw std::runtime_error("Failed to initialize GLAD");
    }
    
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSetWindowUserPointer(mWindow.get(), this); // set the window to reference this class
    glViewport(0, 0, spec.WindowWidth, spec.WindowHeight); // specify size of viewport
    SetCallbacks();

  // render target init
  Graphics::FramebufferSpec framebufferSpec;
  framebufferSpec.width = spec.WindowWidth;
  framebufferSpec.height = spec.WindowHeight;
  framebufferSpec.attachments = { Graphics::FramebufferTextureFormat::RGBA8, Graphics::FramebufferTextureFormat::DEPTH };

  mRenderTargets.emplace_back(framebufferSpec); //EditorView
  mRenderTargets.emplace_back(framebufferSpec); //GameView

  mRenderTargets.front().camera = Graphics::EditorCamera(
      glm::vec3(0.0f, 5.0f, 10.0f),  // Position
      -90.0f,                        // Yaw
      -30.0f,                        // Pitch (look downwards slightly)
      60.0f,                         // FOV
      16.0f / 9.0f,                  // Aspect Ratio
      0.1f,                          // Near Clip
      1500.0f                         // Far Clip
    );
}

  void Application::SetCallbacks() {
    glfwSetFramebufferSizeCallback(mWindow.get(), FramebufferSizeCallback);
    glfwSetErrorCallback(ErrorCallback);
  }

  void Application::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    for (auto& target : app->mRenderTargets) {
      target.framebuffer->Resize(width, height);
    }

    QUEUE_EVENT(Events::WindowResized, width, height);
  }

  void Application::ErrorCallback(int err, const char* desc) {
    UNREFERENCED_PARAMETER(err);
#ifdef _DEBUG
    std::cerr << "GLFW ERROR: \"" << desc << "\"" << " | Error code: " << std::endl;
#endif
  }

  void Application::Shutdown()
  {
    // shutdown singletons
    Systems::SystemManager::DestroyInstance();
    IGE::Audio::AudioManager::DestroyInstance();
    Graphics::PostProcessingManager::DestroyInstance();
    Scenes::SceneManager::DestroyInstance();
    Prefabs::PrefabManager::DestroyInstance();
    Mono::ScriptManager::DestroyInstance();
    Reflection::ObjectFactory::DestroyInstance();
    Performance::FrameRateController::DestroyInstance();
    Events::EventManager::DestroyInstance();
    Layers::LayerManager::DestroyInstance();
    ECS::EntityManager::DestroyInstance();
    Input::InputManager::DestroyInstance();
    Assets::AssetManager::DestroyInstance();
    Debug::DebugLogger::DestroyInstance();

    // @TODO: Shutdown physics and audio singletons
  }

  void Application::ToggleFullscreen(){
      if (mWindowState.isFullscreen) {
          glfwSetWindowMonitor(mWindow.get(), nullptr, mWindowState.windowedPosX, mWindowState.windowedPosY, mWindowState.windowedWidth, mWindowState.windowedHeight, 0);
          mWindowState.isFullscreen = false;
      }
      else {
          glfwGetWindowPos(mWindow.get(), &mWindowState.windowedPosX, &mWindowState.windowedPosY);
          glfwGetWindowSize(mWindow.get(), &mWindowState.windowedWidth, &mWindowState.windowedHeight);

          GLFWmonitor* monitor = glfwGetPrimaryMonitor();
          const GLFWvidmode* mode = glfwGetVideoMode(monitor);
          glfwSetWindowMonitor(mWindow.get(), monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
          mWindowState.isFullscreen = true;
      }

      // Update framebuffers
      int width, height;
      glfwGetFramebufferSize(mWindow.get(), &width, &height);
      glViewport(0, 0, width, height);
      //for (auto& target : mRenderTargets) {
      //    target.framebuffer->Resize(width, height);
      //}
      Graphics::Renderer::ResizeFinalFramebuffer(width, height);
  }

  Application::~Application()
  {
    mWindow.reset();  // release the GLFWwindow before we terminate
    glfwTerminate();
  }
} // namespace IGE
