#include <pch.h>
#include "AssetBrowser.h"
#include <imgui/imgui.h>

namespace GUI
{

  AssetBrowser::AssetBrowser(std::string const& name) : GUIWindow(name) {}

  void AssetBrowser::Run()
  {
    ImGui::Begin(mWindowName.c_str());

    

    ImGui::End();
  }

} // namespace GUI
