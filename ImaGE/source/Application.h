#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <FrameRateController/FrameRateController.h>
#include <TempScene.h>
#include <memory>
#include <Graphics/Framebuffer.h>

class Application
{
public:
  Application(const char* name, int width, int height);
  ~Application();

  Application(Application const&) = delete;
  Application& operator=(Application const&) = delete;

  void Init();
  void Run();

private:
  using SceneDrawCall = std::function<void()>;
  std::unique_ptr<Scene> mScene;
  // vector of framebuffers to render to, each attached to a draw call
  std::vector<std::pair<Graphics::Framebuffer, SceneDrawCall>> mFramebuffers;

  GLFWwindow* mWindow;
  int mWidth, mHeight;
  bool mImGuiActive;

  void ImGuiStartFrame() const;
  void UpdateFramebuffers();
  void SetCallbacks();

  static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
  static void ErrorCallback(int err, const char* desc);

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
  static void WindowDropCallback(GLFWwindow*, int pathCount, const char* paths[]);

  inline void ToggleImGuiActive() noexcept { mImGuiActive = !mImGuiActive; }
};
