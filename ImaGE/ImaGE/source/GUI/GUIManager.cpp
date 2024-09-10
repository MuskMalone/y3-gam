#include <pch.h>
#include "GUIManager.h"
#include "Elements/ObjectManager.h"
#include "Elements/ObjectEditor.h"
#include "Elements/SettingsEditor.h"
#include "Elements/TreeConfig.h"
#include <ImGui/imgui.h>

namespace GUI
{
  std::vector<std::unique_ptr<GUIWindow>> GUIManager::m_windows;
  bool GUIManager::m_isPopupShown{ false };

  void GUIManager::Init(Scene& scene)
  {
    m_windows.reserve(4);
    m_windows.emplace_back(std::make_unique<ObjectManager>("Object Manager", scene));
    m_windows.emplace_back(std::make_unique<ObjectEditor>("Object Editor", scene));
    m_windows.emplace_back(std::make_unique<TreeConfig>("Tree Config", scene, scene.m_bvh));
    m_windows.emplace_back(std::make_unique<SettingsEditor>("Settings", scene));
  }

  void GUIManager::UpdateGUI()
  {
    if (ImGui::IsKeyPressed(ImGuiKey_MouseRight))
    {
      ImGui::OpenPopup("RightClickMenu"); //m_isPopupShown = true;
    }

    // run all actrive windows
    for (auto& window : m_windows)
    {
      if (window->IsDisabled()) { continue; }

      window->Run();
    }

    UpdatePopUpMenu();
  }

  void GUIManager::UpdatePopUpMenu()
  {
    if (ImGui::BeginPopup("RightClickMenu"))
    {
      for (auto& window : m_windows)
      {
        if (ImGui::MenuItem(window->GetName().c_str(), nullptr, !window->IsDisabled()))
        {
          window->Toggle();
        }
      }

      ImGui::EndPopup();
    }
  }

} // namespace GUI
