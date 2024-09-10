#include <pch.h>
#include "Application.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <GUI/GUIManager.h>
#include <Input/InputAssistant.h>

void Application::Init()
{
  m_scene->Init();
  GUI::GUIManager::Init(*m_scene);

  InputAssistant::RegisterKeyPressEvent(GLFW_KEY_GRAVE_ACCENT, std::bind(&Application::ToggleImGuiActive, this));
}

void Application::Run()
{
  while (!glfwWindowShouldClose(m_window))
  {
    m_frc.Update();
    glfwPollEvents();

    if (m_imGuiActive)
    {
      // Start the Dear ImGui frame
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();
#ifdef _DEBUG
      //ImGui::ShowDemoWindow(); // Show demo window! :)
#endif
    }

    if (m_imGuiActive)
    {
      GUI::GUIManager::UpdateGUI();
    }
    InputAssistant::Update();
    m_scene->Update(m_frc.GetDeltaTime());

    glBindFramebuffer(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT);
    m_scene->Draw();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (m_imGuiActive)
    {
      for (auto const& [fb, drawFn] : m_framebuffers)
      {
        fb.Bind();

        drawFn();

        fb.Unbind();

        ImGui::Begin(fb.GetName().c_str());
        ImGui::Image(
          (ImTextureID)(uintptr_t)fb.GetTextureID(),
          ImGui::GetContentRegionAvail(),
          ImVec2(0, 1),
          ImVec2(1, 0)
        );
        ImGui::End();
      }

      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
      {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
      }
    }

    // check and call events, swap buffers
    glfwSwapBuffers(m_window);
  }
}

Application::Application(const char* name, int width, int height) :
  m_frc{}, m_scene{}, m_window{},
  m_width{ width }, m_height{ height }, m_imGuiActive{ true }
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_RED_BITS, 8); glfwWindowHint(GLFW_GREEN_BITS, 8);
  glfwWindowHint(GLFW_BLUE_BITS, 8); glfwWindowHint(GLFW_ALPHA_BITS, 8);

  m_window = glfwCreateWindow(width, height, name, NULL, NULL);
  if (!m_window) {
    throw std::runtime_error("Unable to create window for application");
  }

  glfwMakeContextCurrent(m_window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    throw std::runtime_error("Failed to initialize GLAD");
  }

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
  ImGui_ImplGlfw_InitForOpenGL(m_window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
  ImGui_ImplOpenGL3_Init();

  glfwSetWindowUserPointer(m_window, this); // set the window to reference this class
  
  glViewport(0, 0, width, height); // specify size of viewport
  SetCallbacks();

  m_scene = std::make_unique<Scene>("./shaders/BlinnPhong.vert.glsl", "./shaders/BlinnPhong.frag.glsl");
  // attach each draw function to its framebuffer
  m_framebuffers.emplace_back(std::piecewise_construct, std::forward_as_tuple("Top-down View", width, height),
    std::forward_as_tuple(std::bind(&Scene::DrawTopView, m_scene.get())));
}

Application::~Application()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
}

void Application::SetCallbacks()
{
  glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);
  glfwSetErrorCallback(ErrorCallback);
  InputAssistant::Init(m_window);
}

void Application::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);

  Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
  for (auto& elem : app->m_framebuffers)
  {
    elem.first.Resize(width, height);
  }
}

void Application::ErrorCallback(int err, const char* desc)
{
#ifdef _DEBUG
  std::cerr << "GLFW ERROR: \"" << desc << "\"" << " | Error code: " << std::endl;
#endif
}
