#include <pch.h>
#include "AssetBrowser.h"
#include <imgui/imgui.h>
#include <Globals.h>

namespace Helper
{
  bool ContainsDirectories(std::filesystem::path const& dirEntry);
}

namespace GUI
{

  AssetBrowser::AssetBrowser(std::string const& name) : mCurrentDir{}, GUIWindow(name) {}

  void AssetBrowser::Run()
  {
    ImGui::Begin(mWindowName.c_str());

    ImGui::BeginChild("DirTree", ImVec2(), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX);

    DirectoryTree();

    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("ContentViewer", ImVec2(), ImGuiChildFlags_Border);

    ContentViewer();

    ImGui::EndChild();

    ImGui::End();

    //ImGui::ShowDemoWindow();
  }

  void AssetBrowser::DirectoryTree()
  {
    if (ImGui::TreeNodeEx("Assets", ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow))
    {
      for (auto const& file : std::filesystem::directory_iterator(gAssetsDirectory))
      {
        if (!file.is_directory()) { continue; }

        RecurseDownDirectory(file);
      }

      ImGui::TreePop();
    }
  }

  void AssetBrowser::ContentViewer()
  {
    if (mCurrentDir.empty()) { return; }

    float const regionAvailX{ 0.8f * ImGui::GetContentRegionAvail().x };
    int const assetsPerRow{ regionAvailX > sMaxAssetSize * 10.f ? 10 : regionAvailX < sMaxAssetSize * 3.f ? 3 : 6 };
    float const sizePerAsset{ regionAvailX / static_cast<float>(assetsPerRow) };
    float const imgSize{ sizePerAsset > sMaxAssetSize ? sMaxAssetSize : sizePerAsset };

    unsigned maxChars{ static_cast<unsigned>(imgSize * 0.9f / ImGui::CalcTextSize("L").x) };
    if (ImGui::BeginTable("DirectoryTable", assetsPerRow, ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY))
    {
      for (auto const& file : std::filesystem::directory_iterator(mCurrentDir)) {
        std::string const fileName{ file.path().filename().string() };
        if (file.is_directory()) { continue; }
        ImGui::TableNextColumn();
        ImGui::ImageButton(0, ImVec2(imgSize, imgSize));
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 0.05f * imgSize);
        ImGui::Text((fileName.size() > maxChars ? fileName.substr(0, maxChars - 2) + "..." : fileName).c_str());
      }

      ImGui::EndTable();
    }
  }

  void AssetBrowser::RecurseDownDirectory(std::filesystem::path const& path)
  {
    std::string const fileName{ path.filename().string() };
    bool const hasDirectories{ Helper::ContainsDirectories(path) };
    ImGuiTreeNodeFlags flag{ ImGuiTreeNodeFlags_SpanFullWidth };
    flag |= hasDirectories ? ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick : ImGuiTreeNodeFlags_Leaf;
    if (path == mCurrentDir) { flag |= ImGuiTreeNodeFlags_Selected; }

    if (ImGui::TreeNodeEx(fileName.c_str(), flag))
    {
      if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
      {
        std::cout << "Selected " << path.string() << "\n";
        mCurrentDir = path;
      }

      if (hasDirectories)
      {
        for (auto const& file : std::filesystem::directory_iterator(path))
        {
          if (!file.is_directory()) { continue; }

          RecurseDownDirectory(file);
        }
      }

      ImGui::TreePop();
    }
  }

} // namespace GUI


namespace Helper
{
  bool ContainsDirectories(std::filesystem::path const& dirEntry)
  {
    for (auto const& file : std::filesystem::directory_iterator(dirEntry)) {
      if (file.is_directory()) { return true; }
    }

    return false;
  }
}
