/*!*********************************************************************
\file   AssetBrowser.h
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Class encapsulating functions to run the asset browser of the
        editor. Displays files in the assets directory and allows
        various input operations such as drag-and-drop and right-click
        options.
  
Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
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
    AssetBrowser(const char* name);

    void Run() override;

  private:
    std::filesystem::path mCurrentDir, mRightClickedDir; 
    std::filesystem::path mSelectedAsset;
    bool mDirMenuPopup, mAssetMenuPopup;
    bool mDisableSceneChange, mDisablePrefabSpawn;

    static constexpr float sMaxAssetSize = 100.f;
    static inline constexpr char sMeshPopupTitle[] = "Mesh Import Config";

    /*!*********************************************************************
    \brief
      Forwards any non-mesh imports to the asset manager, then render a
      popup for the user to select mesh-import settings before sending
      them to the asset manager
    ************************************************************************/
    EVENT_CALLBACK_DECL(FilesImported);

    /*!*********************************************************************
    \brief
      Runs the menu bar of the asset browser
    ************************************************************************/
    void MenuBar();

    /*!*********************************************************************
    \brief
      Runs the directory tree of the asset browser to show the list of
      directories in the assets folder
    ************************************************************************/
    void DirectoryTree();

    /*!*********************************************************************
    \brief
      Runs the content viewer of the asset browser to diplay the files
      in the current directory
    ************************************************************************/
    void ContentViewer();

    /*!*********************************************************************
    \brief
      Displays the files in the current selected directory
    \param imgSize
      The size of each item in the browser
    \param maxChars
      The max characters each file name can display
    ************************************************************************/
    void DisplayDirectory(float imgSize, unsigned maxChars);

    /*!*********************************************************************
    \brief
      Displays the results if the search bar is being used
    \param imgSize
      The size of each item in the browser
    \param maxChars
      The max characters each file name can display
    ************************************************************************/
    void DisplaySearchResults(float imgSize, unsigned maxChars);

    /*!*********************************************************************
    \brief
      Checks for any input for a file selected. Menu options, DragDrop etc.
    \param path
      The path of the file selected
    ************************************************************************/
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

    /*!*********************************************************************
    \brief
      Runs the popup for importing a mesh
    ************************************************************************/
    void ImportMeshPopup() const;
  };

} // namespace GUI

#endif  // IMGUI_DISABLE
