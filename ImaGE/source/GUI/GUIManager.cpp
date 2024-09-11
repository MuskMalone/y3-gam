#include <pch.h>
#include "GUIManager.h"
#include <ImGui/imgui.h>
#include "Elements/Viewport.h"

namespace GUI
{
  std::vector<std::unique_ptr<GUIWindow>> GUIManager::m_windows;

  void GUIManager::Init(Graphics::Framebuffer const& framebuffer)
  {
    m_windows.reserve(4);
    m_windows.emplace_back(std::make_unique<Viewport>("Viewport", framebuffer));
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
  }

} // namespace GUI
