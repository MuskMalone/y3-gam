#include <pch.h>
#include "Viewport.h"
#include <imgui/imgui.h>
#include <GUI/ImGuiHelpers.h>
#include <sstream>
#include <glm/gtx/transform.hpp>

namespace GUI
{

  Viewport::Viewport(std::string const& name, Graphics::Framebuffer const& framebuffer) : m_framebuffer{ framebuffer }, GUIWindow(name) {}

  void Viewport::Run()
  {
    ImGui::Begin(m_windowName.c_str());

    ImGui::Image(
      (ImTextureID)(uintptr_t)m_framebuffer.GetTextureID(),
      ImGui::GetContentRegionAvail(),
      ImVec2(0, 1),
      ImVec2(1, 0)
    );

    ImGui::End();
  }

} // namespace GUI
