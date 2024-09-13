#include <pch.h>
#include "Application.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <GUI/GUIManager.h>
#include <Input/InputAssistant.h>
#include <Events/EventManager.h>
#include <Scenes/SceneManager.h>

#include <Core/EntityManager.h>
#include <Core/Entity.h>
#include <Core/Component.h>

void Application::Init() {
  mScene->Init();
  GUI::GUIManager::Init(mFramebuffers.front().first);
  Scenes::SceneManager::GetInstance().Init();
  InputAssistant::RegisterKeyPressEvent(GLFW_KEY_GRAVE_ACCENT, std::bind(&Application::ToggleImGuiActive, this));

  // @TODO: SETTINGS TO BE LOADED FROM CONFIG FILE
  FrameRateController::GetInstance().Init(360.f, 1.f, false);
}

void Application::Run() {
  while (!glfwWindowShouldClose(mWindow)) {
    FrameRateController::GetInstance().Start();

    glfwPollEvents();

    if (mImGuiActive) {
      ImGuiStartFrame();
    }

    // @TODO: REPLACE WITH INPUT MANAGER UPDATE
    InputAssistant::Update();

    // dispatch all events in the queue at the start of game loop
    static auto& eventManager{ Events::EventManager::GetInstance() };
    eventManager.DispatchAll();

    if (mImGuiActive) {
      GUI::GUIManager::UpdateGUI();
    }

    mScene->Update(FrameRateController::GetInstance().GetDeltaTime());

    if (mImGuiActive)
    {
      UpdateFramebuffers();

      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      // for floating windows feature
      if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
      {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
      }
    }

    // check and call events, swap buffers
    glfwSwapBuffers(mWindow);

    FrameRateController::GetInstance().End();
  }
}

Application::Application(const char* name, int width, int height) :
  mScene{}, mWindow{},
  mWidth{ width }, mHeight{ height }, mImGuiActive{ true }
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  glfwWindowHint(GLFW_RED_BITS, 8); glfwWindowHint(GLFW_GREEN_BITS, 8);
  glfwWindowHint(GLFW_BLUE_BITS, 8); glfwWindowHint(GLFW_ALPHA_BITS, 8);

  mWindow = glfwCreateWindow(width, height, name, NULL, NULL);
  if (!mWindow) {
    throw std::runtime_error("Unable to create window for application");
  }

  glfwMakeContextCurrent(mWindow);
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
  ImGui_ImplGlfw_InitForOpenGL(mWindow, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
  ImGui_ImplOpenGL3_Init();

  glfwSetWindowUserPointer(mWindow, this); // set the window to reference this class
  
  glViewport(0, 0, width, height); // specify size of viewport
  SetCallbacks();

  mScene = std::make_unique<Scene>("./shaders/BlinnPhong.vert.glsl", "./shaders/BlinnPhong.frag.glsl");
  // attach each draw function to its framebuffer
  mFramebuffers.emplace_back(std::piecewise_construct, std::forward_as_tuple(width, height),
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

void Application::ImGuiStartFrame() const
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::DockSpaceOverViewport(); // convert the window into a dockspace
}

void Application::SetCallbacks()
{
  glfwSetFramebufferSizeCallback(mWindow, FramebufferSizeCallback);
  glfwSetErrorCallback(ErrorCallback);
  InputAssistant::Init(mWindow);
}

void Application::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);

  Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
  for (auto& elem : app->mFramebuffers)
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

Application::~Application()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
}