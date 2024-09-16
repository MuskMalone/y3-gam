#include <pch.h>
#ifndef IMGUI_DISABLE
#include "Viewport.h"
#include <imgui/imgui.h>

namespace GUI
{

  Viewport::Viewport(std::string const& name, Graphics::Framebuffer const& framebuffer) : mFramebuffer{ framebuffer }, GUIWindow(name) {}

  void Viewport::Run()
  {
    ImGui::Begin(mWindowName.c_str());

    ImGui::Image(
      (ImTextureID)(uintptr_t)mFramebuffer.GetTextureID(),
      ImGui::GetContentRegionAvail(),
      ImVec2(0, 1),
      ImVec2(1, 0)
    );

    ImGui::End();
  }

} // namespace GUI

#endif  // IMGUI_DISABLE
