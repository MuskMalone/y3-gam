/*!*********************************************************************
\file   Application.h
\date   5-October-2024
\brief  The main class running the engine. Updates all systems in the
        engine and sets up the GLFW window.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <GLFW/glfw3.h>
#include <External/GLFWwindowDestructor.h>
#include <FrameRateController/FrameRateController.h>
#include <DebugTools/Exception/Exception.h>
#include <Events/EventCallback.h>
#include <memory>

namespace IGE {
  class Application {
  public:
    struct ApplicationSpecification {
      std::string Name = "ImaGE";
      int WindowWidth = WINDOW_WIDTH<int>, WindowHeight = WINDOW_HEIGHT<int>;
      bool Fullscreen = false;
      bool VSync = true;
      bool StartMaximized = true;
      bool Resizable = true;
      bool EnableImGui = true;
      std::pair<bool, std::string> StartFromScene = std::pair<bool, std::string>(false, ""); // First: StartFromScene Flag, Second: Path to Scene
    };

  public:
    using WindowPtr = std::unique_ptr<GLFWwindow, GLFWwindowDestructor>;

    Application() = default;
    Application(ApplicationSpecification spec);
    virtual ~Application();

    Application(Application const&) = delete;
    Application& operator=(Application const&) = delete;


    /*!*********************************************************************
    \brief
      Initializes the application
    ************************************************************************/
    virtual void Init();

    /*!*********************************************************************
    \brief
      Update function of the application
    ************************************************************************/
    virtual void Run();
    
    /*!*********************************************************************
    \brief
      Shuts down the application and releases its resources
    ************************************************************************/
    virtual void Shutdown();

    // getters
    inline static bool IsImGUIActive() { return mSpecification.EnableImGui; }
    inline static ApplicationSpecification GetApplicationSpecification() { return mSpecification; }
    //inline void SetScene(std::unique_ptr<Scene> scene) { mScene = std::move(scene); }
    //inline void SetWindowPointer(Application::WindowPtr window) { mWindow = std::move(window); }
    //inline std::unique_ptr<Scene>& GetScene() { return mScene; }
    //inline std::unique_ptr<GLFWwindow, GLFWwindowDestructor>& GetWindowPointer() { return mWindow; }

    //inline std::vector<std::pair<std::shared_ptr<Graphics::Framebuffer>, SceneDrawCall>>& GetFrameBuffer() { return mFramebuffers; }
    //inline std::vector<std::pair<std::shared_ptr<Graphics::Framebuffer>, SceneDrawCall>> const& GetFrameBuffer() const { return mFramebuffers; }
    void ToggleFullscreen();
    inline static bool GetImGuiEnabled() { return mSpecification.EnableImGui; }
    inline static bool ToggleImGuiEnabled() { return mSpecification.EnableImGui = !mSpecification.EnableImGui; }
  protected:
      struct WindowState {
          int windowedPosX = {}, windowedPosY = {}; // Previous window position
          int windowedWidth = {}, windowedHeight = {}; // Previous window size
          bool isFullscreen = { false }; // Current fullscreen state
      } mWindowState;
  protected:
    // callbacks
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void ErrorCallback(int err, const char* desc);

    WindowPtr mWindow;

    static ApplicationSpecification mSpecification;

  private:
    /*!*********************************************************************
    \brief
      Sets the callbacks for the application
    ************************************************************************/
    void SetCallbacks();

    /*!*********************************************************************
    \brief
      Registers all systems to the SystemManager
    ************************************************************************/
    void RegisterSystems();

    EVENT_CALLBACK_DECL(OnPausedUpdateTrigger);
    EVENT_CALLBACK_DECL(LockMouse);
  };
} // namespace IGE
