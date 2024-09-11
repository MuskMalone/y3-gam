#include <pch.h>
#include "GUIManager.h"
#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#pragma region IndivWindowIncludes
#include "Elements/SceneControls.h"
#include "Elements/Viewport.h"
#include "Elements/Inspector.h"
#include "Elements/SceneHierarchy.h"
#include "Elements/AssetBrowser.h"
#pragma endregion

namespace GUI
{
  std::vector<std::unique_ptr<GUIWindow>> GUIManager::m_windows;

  void GUIManager::Init(Graphics::Framebuffer const& framebuffer)
  {
    m_windows.reserve(5);
    m_windows.emplace_back(std::make_unique<Viewport>("Viewport", framebuffer));
    //m_windows.emplace_back(std::make_unique<SceneControls>("Scene Controls"));
    m_windows.emplace_back(std::make_unique<Inspector>("Inspector"));
    m_windows.emplace_back(std::make_unique<SceneHierarchy>("Scene Hierarchy"));
    m_windows.emplace_back(std::make_unique<AssetBrowser>("Asset Browser"));
  }

  void GUIManager::UpdateGUI()
  {
    RunToolbar();

    // run all active windows
    for (auto& window : m_windows)
    {
      if (!window->IsActive()) { continue; }

      window->Run();
    }
  }

  void GUIManager::RunToolbar()
  {
    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("File"))
      {

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("View"))
      {
        for (auto const& window : m_windows)
        {
          if (ImGui::MenuItem(window->GetName().c_str(), nullptr, window->IsActive()))
          {
            window->Toggle();
          }
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Options"))
      {

        ImGui::EndMenu();
      }


      ImGui::EndMainMenuBar();
    }

    if (ImGui::BeginViewportSideBar("##SceneControls", (ImGuiViewportP*)(void*)ImGui::GetMainViewport(),
      ImGuiDir_Up, ImGui::GetFrameHeight(), ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar))
    {
      if (ImGui::BeginMenuBar())
      {
        static float const xOffset{ (ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("     PlayPauseStep").x) * 0.5f };
        ImGui::SetCursorPosX(xOffset);

        if (ImGui::Button("Play"))
        {

        }

        if (ImGui::Button("Pause"))
        {

        }

        if (ImGui::Button("Step"))
        {

        }

        ImGui::EndMenuBar();
      }
    }
    ImGui::End();
  }

} // namespace GUI
