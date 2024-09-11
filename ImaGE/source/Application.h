#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <FrameRateController/FrameRateController.h>
#include <Scene.h>
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

  FRC m_frc;
  std::unique_ptr<Scene> m_scene;
  // vector of framebuffers to render to, each attached to a draw call
  std::vector<std::pair<Graphics::Framebuffer, SceneDrawCall>> m_framebuffers;

  GLFWwindow* m_window;
  int m_width, m_height;
  bool m_imGuiActive;

  void ImGuiStartFrame() const;
  void UpdateFramebuffers();
  void SetCallbacks();

  static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
  static void ErrorCallback(int err, const char* desc);

  inline void ToggleImGuiActive() noexcept { m_imGuiActive = !m_imGuiActive; }
};
