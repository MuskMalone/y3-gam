#include <pch.h>
#include "Toolbar.h"
#include <imgui/imgui.h>
#include <ImGui/imgui_internal.h> // for BeginViewportSideBar

namespace GUI
{

  Toolbar::Toolbar(std::string const& name, std::vector<std::unique_ptr<GUIWindow>> const& windowsRef)\
  : m_windowsRef{ windowsRef }, GUIWindow(name) {}

  void Toolbar::Run()
  {
    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("File"))
      {

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("View"))
      {
        for (auto const& window : m_windowsRef)
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
