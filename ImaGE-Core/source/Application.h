#pragma once
#include <GLFW/glfw3.h>
#include <External/GLFWwindowDestructor.h>
#include <FrameRateController/FrameRateController.h>
#include <Scenes/Scene.h>
#include <memory>
#include <DebugTools/Exception/Exception.h>
#include <Graphics/Framebuffer.h>
#include <Core/Systems/SystemManager/SystemManager.h>
#include <Graphics/RenderTarget.h>

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
    };

  public:
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
    //inline void SetScene(std::unique_ptr<Scene> scene) { mScene = std::move(scene); }
    //inline void SetWindowPointer(Application::WindowPtr window) { mWindow = std::move(window); }
    //inline std::unique_ptr<Scene>& GetScene() { return mScene; }
    //inline std::unique_ptr<GLFWwindow, GLFWwindowDestructor>& GetWindowPointer() { return mWindow; }

    //inline std::vector<std::pair<std::shared_ptr<Graphics::Framebuffer>, SceneDrawCall>>& GetFrameBuffer() { return mFramebuffers; }
    //inline std::vector<std::pair<std::shared_ptr<Graphics::Framebuffer>, SceneDrawCall>> const& GetFrameBuffer() const { return mFramebuffers; }
    inline static bool GetImGuiEnabled() { return mSpecification.EnableImGui; }

  protected:
    inline Graphics::RenderTarget& GetDefaultRenderTarget() noexcept { return mRenderTargets.front(); }

    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void ErrorCallback(int err, const char* desc);

    Systems::SystemManager mSystemManager;
    std::vector<Graphics::RenderTarget> mRenderTargets;
    WindowPtr mWindow;

    static ApplicationSpecification mSpecification;

  private:
    void SetCallbacks();
    void RegisterSystems();
  };
} // namespace IGE
