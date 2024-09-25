#pragma once
#include <pch.h>
#include <GLFW/glfw3.h>
#include <External/GLFWwindowDestructor.h>
#include <FrameRateController/FrameRateController.h>
#include <TempScene.h>
#include <memory>
#include <DebugTools/Exception/Exception.h>
#include <Graphics/Framebuffer.h>

class Application {
public:
  struct ApplicationSpecification {
    std::string Name = "ImaGE";
    int WindowWidth = WINDOW_WIDTH<int>, WindowHeight = WINDOW_HEIGHT<int>;
    bool Fullscreen = false;
    bool VSync = true;
    bool StartMaximized = true;
    bool Resizable = true;
    bool EnableImGui = false;
  };

public:
  using SceneDrawCall = std::function<void()>;
  using WindowPtr = std::unique_ptr<GLFWwindow, GLFWwindowDestructor>;

  Application() = default;
  Application(ApplicationSpecification spec);
  virtual ~Application();

  Application(Application const&) = delete;
  Application& operator=(Application const&) = delete;

  virtual void Init();
  virtual void Run();
  virtual void Shutdown();

  inline static bool IsImGUIActive() { return mSpecification.EnableImGui; }
  inline static ApplicationSpecification GetApplicationSpecification() { return mSpecification; }
  inline void SetScene(std::unique_ptr<Scene> scene) { mScene = std::move(scene); }
  inline void SetWindowPointer(Application::WindowPtr window) { mWindow = std::move(window); }
  inline std::unique_ptr<Scene>& GetScene() { return mScene; }
  inline std::unique_ptr<GLFWwindow, GLFWwindowDestructor>& GetWindowPointer() { return mWindow; }

  inline std::vector<std::pair<Graphics::Framebuffer, SceneDrawCall>>& GetFrameBuffer() { return mFramebuffers; }

private:
  std::unique_ptr<Scene> mScene;
  std::vector<std::pair<Graphics::Framebuffer, SceneDrawCall>> mFramebuffers;

  WindowPtr mWindow;
  static ApplicationSpecification mSpecification;
  virtual void SetCallbacks();

protected:
  static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
  static void ErrorCallback(int err, const char* desc);

  void PrintException(Debug::ExceptionBase& e);
  void PrintException(std::exception& e);

  void UpdateFramebuffers();
};