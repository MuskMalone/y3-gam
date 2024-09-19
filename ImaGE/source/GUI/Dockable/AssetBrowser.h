#pragma once
#ifndef IMGUI_DISABLE
#include <GUI/GUIWindow.h>
#include <string>
#include <filesystem>
#include <Events/EventCallback.h>

namespace GUI
{

  class AssetBrowser : public GUIWindow
  {
  public:
    AssetBrowser(std::string const& name);

    void Run() override;

    static constexpr char sAssetDragDropPayload[] = "ASSET";

  private:
    std::filesystem::path mCurrentDir, mRightClickedDir; 
    std::filesystem::path mSelectedAsset;
    std::string mSearchQuery;
    bool mDirMenuPopup, mAssetMenuPopup;
    bool mDisableSceneChange;

    static constexpr float sMaxAssetSize = 100.f;

    /*!*********************************************************************
    \brief
      Handles the events the AssetBrowser subscribed to.

      SCENE_STATE_CHANGE
        - Disables certain menu options based on the current scene state
      ADD_FILES
        - Adds a list of files into the engine from the explorer
    \param event
      The event to handle
    ************************************************************************/
    EVENT_CALLBACK_DECL(HandleEvent);

    void MenuBar();
    void DirectoryTree();
    void ContentViewer();
    void DisplayDirectory(float imgSize, unsigned maxChars);
    void DisplaySearchResults(float imgSize, unsigned maxChars);
    void CheckInput(std::filesystem::path const& path);

    /*!*********************************************************************
    \brief
      Traverses the directory passed in and created child nodes for
      directories within it
    \param filepath
      Parent directory i.e. current directory
    ************************************************************************/
    void RecurseDownDirectory(std::filesystem::path const& dirEntry);

    /*!*********************************************************************
    \brief
      Copies the filepaths in the container into their respective
      directories into the currently browsed directory.
    \param files
      The container of filepahts
    ************************************************************************/
    void AddAssets(std::vector<std::string> const& files);

    /*!*********************************************************************
    \brief
      Creates the popup menu for when a directory is right-clicked upon
    ************************************************************************/
    void DirectoryMenuPopup() const;

    /*!*********************************************************************
    \brief
      Creates the popup menu for when an asset is right-clicked upon
    ************************************************************************/
    void AssetMenuPopup();

    /*!*********************************************************************
    \brief
      Creates the confirmation popup for when an asset is deleted
    ************************************************************************/
    void ConfirmDeletePopup() const;
  };

} // namespace GUI

#endif  // IMGUI_DISABLE
