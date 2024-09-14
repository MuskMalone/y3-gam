#pragma once
#include <GUI/GUIWindow.h>
#include <filesystem>

namespace GUI
{

  class AssetBrowser : public GUIWindow
  {
  public:
    AssetBrowser(std::string const& name);

    void Run() override;

  private:
    std::filesystem::path mCurrentDir;  // the currently selected directory

    static constexpr float sMaxAssetSize = 100.f;

    void DirectoryTree();
    void ContentViewer();

    void RecurseDownDirectory(std::filesystem::path const& dirEntry);
  };

} // namespace GUI
