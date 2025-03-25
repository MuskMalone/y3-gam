/*!*********************************************************************
\file   AssetBrowser.cpp
\author chengen.lau\@digipen.edu
\date   5-October-2024
\brief  Class encapsulating functions to run the asset browser of the
        editor. Displays files in the assets directory and allows
        various input operations such as drag-and-drop and right-click
        options.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#include <pch.h>
#include "AssetBrowser.h"
#include <imgui/imgui.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <GUI/Styles/FontAwesome6Icons.h>
#include <thread>

#include "GUI/GUIVault.h"
#include <Scenes/SceneManager.h>
#include <Events/EventManager.h>

#include <Events/AssetEvents.h>
#include <EditorEvents.h>
#include <GUI/Helpers/AssetHelpers.h>
#include <GUI/Helpers/ImGuiHelpers.h>
#include <GUI/Helpers/AssetPayload.h>
#include "Asset/IGEAssets.h"
#include <Graphics/MaterialTable.h>
#include <Graphics/Mesh/MeshImport.h>
#include <Globals.h>

namespace MeshPopup {
  static constexpr float sTableCol1Width = 250.f;
  static std::vector<std::string> sFilesToImport;
  static std::string sMeshPopupInput, sMeshName;
  static bool sOpenMeshPopup{ false }, sMeshOverwriteWarning{ false };
}

namespace
{
  static constexpr ImVec4 sFileIconCol = { 0.3f, 0.3f, 0.3f, 1.f };
  static std::string sSearchQuery, sLowerSearchQuery;
  static bool sRenameMode{ false }, sFirstTimeRename{ true };
  static bool sDeletePopup{ false };

  /*!*********************************************************************
  \brief
    Checks if the given directory contains more directories
  \param dirEntry
    The path of the file
  \return
    True if it contains at least 1 directory and false otherwise
  ************************************************************************/
  bool ContainsDirectories(std::filesystem::path const& dirEntry);

  bool ContainsFiles(std::filesystem::path const& directory);

  /*!*********************************************************************
  \brief
    Converts a string to lower case
  \param str
    The string
  \return
    The string in lower case
  ************************************************************************/
  std::string ToLower(std::string const& str);

  void DisplayFolderIcon(std::filesystem::path const& path);
  void DisplayTempFileIcon(std::string const& ext);
  void MoveAssetDragDropTarget(std::filesystem::path const& path);

  void NextRowTable(const char* label);
  bool IsAssetFile(std::string const& file);
  bool ShouldHideDirectory(std::string const& dirName);
  void DeleteAssetDependencies(std::filesystem::path const& path);
  std::string ChangeAssetPath(GUI::AssetPayload const& asset, std::filesystem::path const& targetDirectory);
}

namespace GUI
{

  AssetBrowser::AssetBrowser(const char* name) : GUIWindow(name),
    mCurrentDir{ gAssetsDirectory }, mRightClickedDir{},
    mSelectedAsset{}, mDirMenuPopup{ false }, mAssetMenuPopup{ false }
  {
    SUBSCRIBE_CLASS_FUNC(Events::AddFilesFromExplorerEvent, &AssetBrowser::FilesImported, this);
    SUBSCRIBE_CLASS_FUNC(Events::RenameAsset, &AssetBrowser::OnRenameFile, this);
  }

  void AssetBrowser::Run()
  {
    ImGui::Begin(mWindowName.c_str(), 0, ImGuiWindowFlags_MenuBar);
    MenuBar();

    ImGui::BeginChild("DirTree", ImVec2(), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX);

    DirectoryTree();

    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("ContentViewer", ImVec2(), ImGuiChildFlags_Border);

    ContentViewer();

    ImGui::EndChild();

    ImGui::End();

    if (MeshPopup::sOpenMeshPopup) {
      ImGui::OpenPopup(sMeshPopupTitle);
      MeshPopup::sOpenMeshPopup = false;
    }
    ImportMeshPopup();
  }

  void AssetBrowser::MenuBar()
  {
    ImGui::BeginMenuBar();
    bool const isSearching{ !sSearchQuery.empty() };
    float const wWidth{ ImGui::GetWindowWidth() };
    
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 70));
    if (ImGui::Button(ICON_FA_PLUS " Add")) {
      auto const files{ AssetHelpers::SelectFilesFromExplorer("Add Files") };

      if (!files.empty()) {
        QUEUE_EVENT(Events::RegisterAssetsEvent, files);
      }
    }
    ImGui::PopStyleColor();

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
      ImGui::SetTooltip("Add Files");
    }

    if (isSearching) {
      ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(255, 255, 255, 155));
      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
    }
    else {
      ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 255));
    }
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 25.f);

    ImGui::SetCursorPosX(wWidth - wWidth * 0.25f);
    ImGui::PushItemWidth(wWidth * 0.24f);
    if (ImGui::InputTextWithHint("##SearchBar", ICON_FA_MAGNIFYING_GLASS " Search Assets", &sSearchQuery, ImGuiInputTextFlags_AutoSelectAll)) {
      sLowerSearchQuery = ToLower(sSearchQuery);
    }
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();
    if (isSearching) {
      ImGui::PopStyleColor(2);
    }
    else {
      ImGui::PopStyleColor();
    }

    ImGui::EndMenuBar();
  }

  void AssetBrowser::DirectoryTree()
  {
    ImGuiTreeNodeFlags flags{ ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow };
    if (mCurrentDir == gAssetsDirectory) { flags |= ImGuiTreeNodeFlags_Selected; }
    if (ImGui::TreeNodeEx("Assets", flags))
    {
      CheckDirectoryInput(gAssetsDirectory, 0.f);

      for (auto const& file : std::filesystem::directory_iterator(gAssetsDirectory))
      {
        if (!file.is_directory() || ShouldHideDirectory(file.path().filename().string())) { continue; }

        RecurseDownDirectory(file, ImGui::GetTreeNodeToLabelSpacing());
      }

      ImGui::TreePop();
    }

    if (mDirMenuPopup) {
      ImGui::OpenPopup(sDirMenuTitle);
      mDirMenuPopup = false;
    }
    else if (sDeletePopup) {
      ImGui::OpenPopup(sDeletePopupTitle);
      sDeletePopup = false;
    }
    DirectoryMenuPopup();
    ConfirmDeletePopup();
  }


  void AssetBrowser::ContentViewer()
  {
    if (!mCurrentDir.empty()) {
      float const regionAvailX{ 0.8f * ImGui::GetContentRegionAvail().x };
      int const assetsPerRow{ regionAvailX > sMaxAssetSize * 10.f ? 10 : regionAvailX < sMaxAssetSize * 4.f ? 3 : 6 };
      float const sizePerAsset{ regionAvailX / static_cast<float>(assetsPerRow) };
      float const imgSize{ sizePerAsset > sMaxAssetSize ? sMaxAssetSize : sizePerAsset };

      unsigned const maxChars{ static_cast<unsigned>(imgSize * 0.9f / ImGui::CalcTextSize("L").x) };
      if (ImGui::BeginTable("DirectoryTable", assetsPerRow, ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY))
      {
        bool const showIcon{ sizePerAsset > 40.f };
        if (sSearchQuery.empty()) {
          DisplayDirectory(imgSize, maxChars, showIcon);
        }
        else {
          DisplaySearchResults(imgSize, maxChars, showIcon);
        }
        ImGui::EndTable();
      }
    }

    if (mAssetMenuPopup) {
      ImGui::OpenPopup(sAssetsMenuTitle);
      mAssetMenuPopup = false;
    }
    else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)) {
      ImGui::OpenPopup(sContentViewerMenuTitle);
    }
    AssetMenuPopup();
    ContentViewerPopup();
  }

  EVENT_CALLBACK_DEF(AssetBrowser, OnRenameFile) {
    auto renameEvent{ CAST_TO_EVENT(Events::RenameAsset) };
    std::filesystem::path const& original{ renameEvent->mOriginal };
    std::filesystem::path newPath{ std::filesystem::path(original).replace_filename(renameEvent->mNewFile) };

    // add extension if missing
    if (!newPath.has_extension()) {
      newPath.replace_extension(original.extension());
    }

    // @TODO: REFACTOR - SHOULD KNOW ASSET TYPE FROM ASSETMGR SIDE
    IGE::Assets::AssetManager& am{ IGE_ASSETMGR };
    std::string const ext{ newPath.extension().string() };
    if (std::filesystem::is_regular_file(original)) {
      if (ext == gPrefabFileExt) {
        am.ChangeAssetPath<IGE::Assets::PrefabAsset>(am.LoadRef<IGE::Assets::PrefabAsset>(original.string()), newPath.string());
      }
      else if (std::string(gMeshFileExt).find(ext) != std::string::npos) {
        am.ChangeAssetPath<IGE::Assets::ModelAsset>(am.LoadRef<IGE::Assets::ModelAsset>(original.string()), newPath.string());
      }
      else if (ext == gMaterialFileExt) {
        IGE::Assets::GUID const guid{ am.LoadRef<IGE::Assets::MaterialAsset>(original.string()) };
        am.ChangeAssetPath<IGE::Assets::MaterialAsset>(guid, newPath.string());
        auto const str{ am.GetAsset<IGE::Assets::MaterialAsset>(guid)->mMaterial };
        am.GetAsset<IGE::Assets::MaterialAsset>(guid)->mMaterial->SetName(newPath.stem().string());
      }
      else if (ext == gAnimationFileExt) {
        am.ChangeAssetPath<IGE::Assets::AnimationAsset>(am.LoadRef<IGE::Assets::AnimationAsset>(original.string()), newPath.string());
      }
      else if (ext == gSpriteFileExt) {
        am.ChangeAssetPath<IGE::Assets::TextureAsset>(am.LoadRef<IGE::Assets::TextureAsset>(original.string()), newPath.string());
      }
      else if (ext == gFontFileExt) {
        am.ChangeAssetPath<IGE::Assets::FontAsset>(am.LoadRef<IGE::Assets::FontAsset>(original.string()), newPath.string());
      }
      else if (std::string(gSupportedAudioFormats).find(ext) != std::string::npos) {
        am.ChangeAssetPath<IGE::Assets::AudioAsset>(am.LoadRef<IGE::Assets::AudioAsset>(original.string()), newPath.string());
      }
      else if (ext == gSceneFileExt) {
        // rename hierarchy state file of scene
        std::string const editorScenesDir{ gEditorAssetsDirectory + std::string(".Scenes\\") };
        // also rename the editor scene config file
        std::filesystem::rename(editorScenesDir + original.stem().string(),
          editorScenesDir + newPath.stem().string());
      }
      GUIVault::SetSelectedFile(newPath);
    }
    else {
      // if directory is currently being traversed, update it as well
      if (original == mCurrentDir) {
        mCurrentDir = newPath;
      }
    }

    std::filesystem::rename(original, newPath);
    IGE_DBGLOGGER.LogInfo("Successfully renamed " + original.filename().string() + " to " + newPath.filename().string());
  }

  void AssetBrowser::DisplayDirectory(float imgSize, unsigned maxChars, bool showIcon)
  {
    if (!std::filesystem::exists(mCurrentDir)) {
      IGE_DBGLOGGER.LogWarning("[AssetBrowser] " + mCurrentDir.string() + " no longer exists!");
      mCurrentDir = gAssetsDirectory;
    }

    for (auto const& file : std::filesystem::directory_iterator(mCurrentDir)) {
      if (!IsAssetFile(file.path().string())) { continue; } // to account for igemeta files

      std::string const fileName{ file.path().filename().string() };
      if (ShouldHideDirectory(fileName)) { continue; }

      ImGui::TableNextColumn();
      bool const exceed{ fileName.size() > maxChars };
      ImVec2 const imgBtnStart{ ImGui::GetCursorPos() };

      // asset icon + input
      ImGui::ImageButton(0, ImVec2(imgSize, imgSize));
      ImVec2 const originalPos{ ImGui::GetCursorPos() };
      ImVec2 elemSize{ ImGui::GetItemRectSize() };

      if (sRenameMode && file == mSelectedAsset) {
        static std::string cpy;

        if (sFirstTimeRename) {
          sFirstTimeRename = false;
          ImGui::SetKeyboardFocusHere();
          cpy = fileName;
        }

        bool saved{ false };
        ImGui::InputText("##renameInput", &cpy, ImGuiInputTextFlags_AutoSelectAll);

        if (ImGui::IsItemHovered() || (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)
            && !ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsMouseClicked(ImGuiMouseButton_Right))) {
          ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
        }
        else {
          if (file.path().filename() != cpy) {
            // execute the rename at the start of the frame since we're traversing assets dir
            QUEUE_EVENT(Events::RenameAsset, file, std::move(cpy));
          }

          cpy.clear();
          sRenameMode = false;
          sFirstTimeRename = saved = true;
        }

        if (!saved && (ImGui::IsKeyPressed(ImGuiKey_Enter))) {
          if (file.path().filename() != cpy) {
            // execute the rename at the start of the frame since we're traversing assets dir
            QUEUE_EVENT(Events::RenameAsset, file, std::move(cpy));
          }

          cpy.clear();
          sRenameMode = false;
          sFirstTimeRename = true;
        }
      }
      else {
        CheckFileInput(file);

        // display file name below
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 0.05f * imgSize);
        ImGui::Text((exceed ? fileName.substr(0, maxChars) : fileName).c_str());

        if (exceed) {
          ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 0.05f * imgSize);
          std::string const secondRow{ fileName.substr(maxChars) };
          ImGui::Text((secondRow.size() > maxChars ? secondRow.substr(0, maxChars - 2) + "..." : secondRow).c_str());
        }
        elemSize.y = ImGui::GetCursorPos().y - imgBtnStart.y;
      }

      if (showIcon) {
        float const halfImgSize{ imgSize * 0.5f };
        ImVec2 const offset{ ImVec2(halfImgSize - 7.f, -halfImgSize - 23.f) };
        ImGui::SetCursorPos(originalPos + offset);

        if (file.is_directory()) {
          DisplayFolderIcon(file.path());
        }
        else {
          DisplayTempFileIcon(file.path().extension().string());
        }
        
        ImGui::SetCursorPos(originalPos);
      }
      
      if (std::filesystem::is_directory(file)) {
        ImGui::SetCursorPos(imgBtnStart);
        ImGui::SetNextItemAllowOverlap();

        ImGui::InvisibleButton(("##" + file.path().string()).c_str(), elemSize);
        MoveAssetDragDropTarget(file);

        ImGui::SetCursorPos(originalPos);
      }

      ImGui::NewLine();
    }
  }

  void AssetBrowser::DisplaySearchResults(float imgSize, unsigned maxChars, bool showIcon)
  {
    for (auto const& file : std::filesystem::recursive_directory_iterator(gAssetsDirectory))
    {
      std::filesystem::path const& path{ file.path() };
      if (!IsAssetFile(path.string())) { continue; } // to account for igemeta files
      // dont show hidden folders or files within it
      else if ((file.is_directory() && ShouldHideDirectory(path.filename().string()))
          || ShouldHideDirectory(path.parent_path().filename().string())) { continue; }

      std::string const fileName{ path.filename().string() };
      if (ToLower(fileName).find(sLowerSearchQuery) == std::string::npos) { continue; }

      ImGui::TableNextColumn();
      bool const exceed{ fileName.size() > maxChars };

      // asset icon + input
      ImGui::ImageButton(0, ImVec2(imgSize, imgSize));

      ImVec2 const originalPos{ ImGui::GetCursorPos() };

      CheckFileInput(path);

      // display file name below
      ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 0.05f * imgSize);
      ImGui::Text((exceed ? fileName.substr(0, maxChars) : fileName).c_str());
      if (exceed) {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 0.05f * imgSize);
        std::string const secondRow{ fileName.substr(maxChars) };
        ImGui::Text((secondRow.size() > maxChars ? secondRow.substr(0, maxChars - 2) + "..." : secondRow).c_str());
      }

      if (showIcon) {
        float const halfImgSize{ imgSize * 0.5f };
        ImVec2 const offset{ ImVec2(halfImgSize - 7.f, -halfImgSize - 23.f) };
        ImGui::SetCursorPos(originalPos + offset);

        if (file.is_directory()) {
          DisplayFolderIcon(file.path());
        }
        else {
          DisplayTempFileIcon(file.path().extension().string());
        }
        ImGui::SetCursorPos(originalPos);
      }

      ImGui::NewLine();
    }
  }

  void AssetBrowser::CheckFileInput(std::filesystem::path const& path)
  {
    static std::filesystem::path draggedAsset;
    
    if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
    {
      mSelectedAsset = path;
      mAssetMenuPopup = true;
    }
    else if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
    {
      if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        if (std::filesystem::is_directory(path)) {
          mCurrentDir = path.string();
        }
        else {
          AssetHelpers::OpenFileWithDefaultProgram(path.string());
        }
      }
      else {
        draggedAsset = path;
      }
    }
    // @TODO: temp
    else if (ImGui::IsItemClicked(ImGuiMouseButton_Middle)) {
      GUIVault::SetSelectedFile(path);
    }
    else if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
      ImGui::SetTooltip(path.filename().string().c_str());
    }

    if (ImGui::BeginDragDropSource()) {
      std::string const pathStr{ draggedAsset.relative_path().string() };

      ImGui::SetDragDropPayload(AssetPayload::sAssetDragDropPayload, pathStr.data(), pathStr.size() + 1, ImGuiCond_Once);
      ImGui::Text(draggedAsset.filename().string().c_str());

      ImGui::EndDragDropSource();
    }
  }
  
  void ImportVideo(std::string const& path) {
    if (!std::filesystem::exists(gVideosDirectory)) {
      std::filesystem::create_directory(gVideosDirectory);
    }
    std::string compiledPath{ std::string(gVideosDirectory) + "Compiled\\" };
    if (!std::filesystem::exists(compiledPath)) {
      std::filesystem::create_directory(compiledPath);
    }

    compiledPath += std::filesystem::path(path).stem().string() + ".mpg";

    // if alread mpg file, just copy over
    if (std::filesystem::path(path).extension() == ".mpg") {
      std::filesystem::copy(path, compiledPath, std::filesystem::copy_options::overwrite_existing);
      IGE_ASSETMGR.ImportAsset<IGE::Assets::VideoAsset>(path);
      IGE_DBGLOGGER.LogInfo("Successfuly imported video to " + compiledPath);
      return;
    }

    // from https://github.com/phoboslab/pl_mpeg:
    // ffmpeg -i input.mp4 -c:v mpeg1video -q:v 0 -c:a libtwolame -b:a 224k -format mpeg output.mpg
   
    // create a detached thread to import the video in background
    std::thread(
      [path, compiledPath]() {
        std::string const cmd{
          "..\\ImaGE-Editor\\source\\Executables\\ffmpeg.exe -i \"" + 
          path + "\" -c:v mpeg1video -q:v 0 -c:a libtwolame -b:a 224k -f mpeg \"" + compiledPath + "\""
        };

        if (std::filesystem::exists(compiledPath)) {
          std::filesystem::remove(compiledPath);
        }

        int result{ std::system(cmd.c_str()) };
        if (result != 0) {
          IGE_DBGLOGGER.LogError("Unable to import " + path);
          return;
        }

        IGE_ASSETMGR.ImportAsset<IGE::Assets::VideoAsset>(path);
        IGE_DBGLOGGER.LogInfo("Successfuly imported video to " + compiledPath);
      }
    ).detach();
  }

  void HandleNextAssetImport() {
    std::filesystem::path const nextAsset{ MeshPopup::sFilesToImport.back() };

    // for now the only 2 types to handle are video and models
    if (std::string(gSupportedVideoFormats).find(nextAsset.extension().string()) != std::string::npos) {
      ImportVideo(MeshPopup::sFilesToImport.back());
      MeshPopup::sFilesToImport.pop_back();

      if (!MeshPopup::sFilesToImport.empty()) {
        HandleNextAssetImport();
      }
    }
    // else here means model
    else {
      MeshPopup::sOpenMeshPopup = true;
      MeshPopup::sMeshName = MeshPopup::sMeshPopupInput = nextAsset.filename().string();
      MeshPopup::sMeshOverwriteWarning = std::filesystem::exists(gMeshOutputDir + MeshPopup::sMeshPopupInput);
    }
  }

  EVENT_CALLBACK_DEF(AssetBrowser, FilesImported) {
    auto const& files{ CAST_TO_EVENT(Events::AddFilesFromExplorerEvent)->mPaths };
    std::vector<std::string> filesToRegister{};

    for (std::string const& file : files) {
      std::filesystem::path const path{ file };
      std::string const ext{ path.extension().string() };
      if (std::string(gSupportedVideoFormats).find(ext) != std::string::npos 
        || std::string(gSupportedModelFormats).find(ext) != std::string::npos) {
        MeshPopup::sFilesToImport.emplace_back(file);
      }
      else {
        filesToRegister.emplace_back(file);
      }
    }

    if (!filesToRegister.empty()) {
      QUEUE_EVENT(Events::RegisterAssetsEvent, std::move(filesToRegister));
    }

    if (!MeshPopup::sFilesToImport.empty()) {
      HandleNextAssetImport();
    }
  }

  void AssetBrowser::RecurseDownDirectory(std::filesystem::path const& path, float nodeToLabelSpacing)
  {
    std::string const fileName{ path.filename().string() };
    bool const hasDirectories{ ContainsDirectories(path) };
    ImGuiTreeNodeFlags flag{ ImGuiTreeNodeFlags_SpanFullWidth };
    flag |= hasDirectories ? ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick : ImGuiTreeNodeFlags_Leaf;
    if (path == mCurrentDir) { flag |= ImGuiTreeNodeFlags_Selected; }

    if (ImGui::TreeNodeEx(fileName.c_str(), flag))
    {
      CheckDirectoryInput(path, nodeToLabelSpacing);

      if (hasDirectories)
      {
        for (auto const& file : std::filesystem::directory_iterator(path))
        {
          if (!file.is_directory() || file.path().filename() == sCompiledDirectory) { continue; }

          RecurseDownDirectory(file, nodeToLabelSpacing + ImGui::GetTreeNodeToLabelSpacing());
        }
      }

      ImGui::TreePop();
    }
    else {
      CheckDirectoryInput(path, nodeToLabelSpacing);
    }
  }

  void AssetBrowser::CheckDirectoryInput(std::filesystem::path const& path, float nodeToLabelSpacing) {
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > nodeToLabelSpacing + 25.f) {
      mCurrentDir = path;
    }
    else if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
      mRightClickedDir = path;
      mDirMenuPopup = true;
    }

    MoveAssetDragDropTarget(path);
  }

  void AssetBrowser::ContentViewerPopup() {
    if (!ImGui::BeginPopup(sContentViewerMenuTitle)) { return; }

    if (ImGui::Selectable("New Folder")) {
      mRightClickedDir = mCurrentDir;
      CreateNewFolder();
    }
    ImGui::Separator();

    if (ImGui::Selectable("New Material")) {
      if (!std::filesystem::exists(gMaterialDirectory)) {
        std::filesystem::create_directory(gMaterialDirectory);
      }

      std::string fileName{ "NewMaterial" };

      if (std::filesystem::exists(std::string(gMaterialDirectory) + fileName + gMaterialFileExt)) {
        int i{ 2 };
        std::string newFileName{ fileName };
        // loop until a valid name
        do {
          newFileName = fileName + " (" + std::to_string(i) + ")";
          ++i;
        } while (std::filesystem::exists(std::string(gMaterialDirectory) + newFileName + gMaterialFileExt));
        fileName = std::move(newFileName);
      }
      
      std::string const fullPath{ gMaterialDirectory + fileName + gMaterialFileExt };
      std::ofstream ofs{ fullPath };
      if (ofs) {
        ofs.close();
        mCurrentDir = gMaterialDirectory;
        sRenameMode = true;
        mSelectedAsset = fullPath;
        Graphics::MaterialTable::CreateAndImportMatFile(fileName);
      }
      else {
        IGE_DBGLOGGER.LogError("Unable to create new material file " + fileName);
      }
    }

    ImGui::EndPopup();
  }

  void AssetBrowser::DirectoryMenuPopup()
  {
    if (ImGui::BeginPopup(sDirMenuTitle))
    {
      if (ImGui::Selectable("New Folder")) {
        // set the folder to current directory to facilitate renaming
        mCurrentDir = mRightClickedDir;
        CreateNewFolder();
      }

      if (ImGui::Selectable("Open in File Explorer")) {
        AssetHelpers::OpenDirectoryInExplorer(mRightClickedDir);
      }

      if (ImGui::Selectable("Delete")) {
        mSelectedAsset = mRightClickedDir;
        sDeletePopup = true;
      }
      
      ImGui::EndPopup();
    }
  }

  void AssetBrowser::AssetMenuPopup()
  {
    if (ImGui::BeginPopup(sAssetsMenuTitle))
    {
      if (ImGui::Selectable("Open##AssetMenu")) {
        if (std::filesystem::is_directory(mSelectedAsset)) {
          mCurrentDir = mSelectedAsset.string();
        }
        else {
          AssetHelpers::OpenFileWithDefaultProgram(mSelectedAsset.string());
        }
      }

      if (ImGui::Selectable("Rename##AssetMenu")) {
        sRenameMode = true;
      }

      if (ImGui::Selectable("Open in File Explorer")) {
        AssetHelpers::OpenFileInExplorer(mSelectedAsset);
      }

      std::string const ext{ mSelectedAsset.extension().string() };
      // file specific options
      if (ext == gPrefabFileExt)
      {
        ImGui::BeginDisabled(IGE_SCENEMGR.GetSceneState() & ~(Scenes::STOPPED | Scenes::NO_SCENE));
        if (ImGui::Selectable("Edit Prefab")) {
          QUEUE_EVENT(Events::EditPrefabEvent, mSelectedAsset.stem().string(),
            mSelectedAsset.relative_path().string());
        }
        ImGui::EndDisabled();
      }
      else if (ext == gMaterialFileExt) {
        if (ImGui::Selectable("Edit Material")) {
          GUIVault::SetSelectedFile(mSelectedAsset);
        }
      }
      else if (ext == gAnimationFileExt) {
        if (ImGui::Selectable("Edit Animation")) {
          try {
            QUEUE_EVENT(Events::EditAnimation, IGE_ASSETMGR.PathToGUID(mSelectedAsset.string()));
          }
          catch (Debug::ExceptionBase&) {
            IGE_DBGLOGGER.LogError("Unable to get GUID of " + mSelectedAsset.string());
          }
        }
      }

      if (ImGui::Selectable("Delete##AssetMenu")) {
        sDeletePopup = true;
      }

      ImGui::EndPopup();
    }
  }

  void AssetBrowser::ConfirmDeletePopup()
  {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal(sDeletePopupTitle, NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
      ImGui::Text("Are you sure you want to delete");
      ImGui::SameLine();
      ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), mSelectedAsset.filename().string().c_str());
      ImGui::SameLine();
      ImGui::Text("?");

      ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x * 0.5f - ImGui::CalcTextSize("Yes ").x);
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.65f, 0.f, 1.f));
      if (ImGui::Button("No##Confirm"))
      {
        ImGui::CloseCurrentPopup();
      }
      ImGui::PopStyleColor();

      ImGui::SameLine();
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.f));
      if (ImGui::Button("Yes##Confirm"))
      {
        std::string const path{ mSelectedAsset.relative_path().string() };
        DeleteAssetDependencies(path);

        if (mCurrentDir == mSelectedAsset) {
          mCurrentDir = gAssetsDirectory;
        }
        if (GUIVault::GetSelectedFile() == path) {
          GUIVault::SetSelectedFile({});
        }
        
        std::uintmax_t const count{ std::filesystem::remove_all(mSelectedAsset.relative_path().string().c_str()) };
        IGE_DBGLOGGER.LogInfo("Successfully deleted " + std::to_string(count) + " files");

        ImGui::CloseCurrentPopup();
      }
      ImGui::PopStyleColor();

      ImGui::EndPopup();
    }
  }

  void AssetBrowser::ImportMeshPopup() const {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (!ImGui::BeginPopupModal(sMeshPopupTitle, NULL, ImGuiWindowFlags_AlwaysAutoResize)) { return; }
    static bool blankWarning{ false };
    static Graphics::AssetIO::ImportSettings settings{};
    bool close{ false };

    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.5f, 0.5f, 0.5f, 1.f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
    ImGui::Text("Importing ");
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(124, 252, 0, 255));
    ImGui::Text(MeshPopup::sMeshName.c_str());
    ImGui::PopStyleColor();
    ImGui::NewLine();

    if (blankWarning) {
      ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Path cannot be blank!!!");
    }
    else if (MeshPopup::sMeshOverwriteWarning) {
      ImGui::TextColored(ImVec4(0.99f, 0.82f, 0.09f, 1.0f), "Warning: File already exists.");
      ImGui::TextColored(ImVec4(0.99f, 0.82f, 0.09f, 1.0f), "File will be overwritten!!");
    }

    float const firstColWidth{ 150.f };
    if (ImGui::BeginTable("MeshPopupTable", 2, ImGuiTableFlags_SizingFixedFit)) {
      ImGui::TableSetupColumn("##col0", ImGuiTableColumnFlags_WidthFixed, firstColWidth);
      ImGui::TableSetupColumn("##col1", ImGuiTableColumnFlags_WidthFixed, MeshPopup::sTableCol1Width);
      ImGui::TableNextRow();

      NextRowTable("Import mesh to:");
      NextRowTable("Assets\\Models\\");
      //if (!ImGui::IsAnyItemActive()) { ImGui::SetKeyboardFocusHere(); }
      ImGui::BeginDisabled();
      if (ImGui::InputText("##MeshPathInput", &MeshPopup::sMeshPopupInput)) {
        MeshPopup::sMeshOverwriteWarning = std::filesystem::exists(gMeshOutputDir + MeshPopup::sMeshPopupInput);
        blankWarning = false;
      }
      ImGui::EndDisabled();
      if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("Rename feature coming soon!");
      }

      NextRowTable("");

      bool elementHover{ false };

      // recenter checkbox
      ImGui::AlignTextToFramePadding();
      NextRowTable("Recenter Mesh?");
      if (ImGui::IsItemHovered()) { elementHover = true; }

      ImGui::Checkbox("##RecenterMesh", &settings.recenterMesh);
      if (ImGui::IsItemHovered()) { elementHover = true; }

      if (elementHover) {
        ImGui::SetTooltip("Remap the mesh's vertices so its center is the origin");
        elementHover = false;
      }
      ImGui::TableNextRow();

      // normalize scale checkbox
      ImGui::AlignTextToFramePadding();
      NextRowTable("Normalize Scale?");
      if (ImGui::IsItemHovered()) { elementHover = true; }

      ImGui::Checkbox("##NormalizeScale", &settings.normalizeScale);
      if (ImGui::IsItemHovered()) { elementHover = true; }

      if (elementHover) {
        ImGui::SetTooltip("Rescale the mesh to a unit size");
        elementHover = false;
      }
      ImGui::TableNextRow();

      // flip UVs checkbox
      ImGui::AlignTextToFramePadding();
      NextRowTable("Flip UVs?");
      if (ImGui::IsItemHovered()) { elementHover = true; }

      ImGui::Checkbox("##FlipUVs", &settings.flipUVs);
      ImGui::TableNextRow();

      // static mesh checkbox
      ImGui::AlignTextToFramePadding();
      NextRowTable("Static Mesh");
      if (ImGui::IsItemHovered()) { elementHover = true; }
      
      ImGui::Checkbox("##StaticMesh", &settings.staticMesh);
      if (ImGui::IsItemHovered()) { elementHover = true; }

      if (elementHover) {
        ImGui::SetTooltip("Combine all sub-meshes into a single mesh entity");
        elementHover = false;
      }
      ImGui::EndTable();

      ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.f);
      if (ImGui::TreeNodeEx("Advanced")) {
        if (ImGui::BeginTable("AdvancedTable", 2, ImGuiTableFlags_SizingFixedFit)) {
          ImGui::TableSetupColumn("##c0", ImGuiTableColumnFlags_WidthFixed, firstColWidth);
          ImGui::TableSetupColumn("##c1", ImGuiTableColumnFlags_WidthFixed, MeshPopup::sTableCol1Width);
          ImGui::TableNextRow();

          bool optimizeVertices{ !settings.minimalFlags };

          NextRowTable("Optimize Vertices");
          if (ImGui::IsItemHovered()) { elementHover = true; }

          if (ImGui::Checkbox("##OptimizeVertices", &optimizeVertices)) {
            settings.minimalFlags = !optimizeVertices;
          }
          if (ImGui::IsItemHovered()) { elementHover = true; }

          if (elementHover) {
            ImGui::BeginTooltip();
            ImGui::Text("Attempt to optimize the structure of the mesh during import. On by default.");
            ImGui::Text("Turn this off if you are facing issues with your mesh.");
            ImGui::EndTooltip();
          }

          ImGui::EndTable();
        }
        ImGui::TreePop();
      }

      

    }

    ImGui::NewLine();
    ImGui::SetCursorPosX(0.5f * (ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("Cancel Create ").x));
    if (ImGui::Button("Cancel##Mesh") || ImGui::IsKeyPressed(ImGuiKey_Escape)) {
      close = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Create##Mesh") || ImGui::IsKeyPressed(ImGuiKey_Enter)) {
      // if name is blank / whitespace, reject it
      if (MeshPopup::sMeshPopupInput.find_first_not_of(" ") == std::string::npos) {
        blankWarning = true;
        MeshPopup::sMeshOverwriteWarning = false;
      }
      else {
        // send the relevant file path to the asset manager
        IGE::Assets::AssetMetadata::AssetProps metadata{};
        metadata.metadata = {
          { IMSH_IMPORT_RECENTER, settings.recenterMesh ? "1" : "0" },
          { IMSH_IMPORT_NORM_SCALE, settings.normalizeScale ? "1" : "0" },
          { IMSH_IMPORT_FLIP_UV, settings.flipUVs ? "1" : "0" },
          { IMSH_IMPORT_STATIC, settings.staticMesh ? "1" : "0" },
          { IMSH_IMPORT_MIN_FLAGS, settings.minimalFlags ? "1" : "0" }
        };

        IGE::Assets::AssetManager& am{ IGE_ASSETMGR };
        IGE::Assets::GUID const guid{ am.ImportAsset<IGE::Assets::ModelAsset>(MeshPopup::sFilesToImport.back(), metadata) };

        close = true;
      }
    }

    if (close) {
      blankWarning = MeshPopup::sMeshOverwriteWarning = false;
      // erase the last element
      MeshPopup::sFilesToImport.erase(MeshPopup::sFilesToImport.begin() + MeshPopup::sFilesToImport.size() - 1);
      // if there are still more to import, run the popup again
      if (!MeshPopup::sFilesToImport.empty()) {
        HandleNextAssetImport();
      }
      else {
        MeshPopup::sMeshPopupInput.clear();
      }

      ImGui::CloseCurrentPopup();
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::EndPopup();
  }

  void AssetBrowser::CreateNewFolder() {
    std::string newFolderPath{ mRightClickedDir.string() + "\\New Folder" };
    
    // keep renaming until we get a new file name
    if (std::filesystem::exists(newFolderPath)) {
      std::string const prefix{ newFolderPath + " (" };
      newFolderPath += " (2)";
      for (int i{ 3 }; std::filesystem::exists(newFolderPath); ++i) {
        newFolderPath = prefix + std::to_string(i) + ")";
      }
    }

    if (std::filesystem::create_directory(newFolderPath)) {
      // trigger rename
      mSelectedAsset = newFolderPath;
      sRenameMode = true;
    }
    else {
      IGE_DBGLOGGER.LogError("Unable to create directory: "
        + mRightClickedDir.string() + "\\New Folder");
    }
  }
} // namespace GUI


namespace
{
  void MoveAssetDragDropTarget(std::filesystem::path const& path) {
    if (ImGui::BeginDragDropTarget()) {
      ImGuiPayload const* drop{ ImGui::AcceptDragDropPayload(GUI::AssetPayload::sAssetDragDropPayload) };
      if (drop) {
        GUI::AssetPayload assetPayload{ reinterpret_cast<const char*>(drop->Data) };

        { // if the target dir alr exists, cancel the move operation
          std::string const newPath{ path.string() + "\\" + assetPayload.mPath.filename().string() };
          if (std::filesystem::exists(newPath)) {
            IGE_DBGLOGGER.LogError("Move aborted. Target directory already exists! (" + newPath + ")");
            ImGui::EndDragDropTarget();
            return;
          }
        }

        // ignore if same file
        if (path != assetPayload.mPath) {
          std::string const oldPath{ assetPayload.GetFilePath() },
            newPath{ ChangeAssetPath(assetPayload, path) };

          try {
            // move the file to the new dir
            std::filesystem::copy(oldPath, newPath, std::filesystem::copy_options::recursive);
            std::uintmax_t const count{ std::filesystem::remove_all(oldPath) };

            IGE_DBGLOGGER.LogInfo("Successfully moved " + std::to_string(count) + " files to " + path.string());
          }
          catch (std::filesystem::filesystem_error& e) {
            IGE_DBGLOGGER.LogError(e.what());
          }
        }
      }
      ImGui::EndDragDropTarget();
    }
  }

  bool ContainsDirectories(std::filesystem::path const& dirEntry)
  {
    for (auto const& file : std::filesystem::directory_iterator(dirEntry)) {
      if (ShouldHideDirectory(file.path().filename().string())) { continue; }

      if (file.is_directory()) { return true; }
    }

    return false;
  }

  bool ContainsFiles(std::filesystem::path const& directory) {
    for (auto const& file : std::filesystem::directory_iterator(directory)) {
      if (ShouldHideDirectory(file.path().filename().string())) { continue; }

      return true;
    }

    return false;
  }

  std::string ToLower(std::string const& str) {
    std::string ret{ str };
    for (char& ch : ret) {
      ch = std::tolower(ch);
    }

    return ret;
  }

  void DisplayFolderIcon(std::filesystem::path const& path) {
    ImGui::PushFont(GUI::GUIVault::GetStyler().GetCustomFont(GUI::CustomFonts::ROBOTO_BOLD));
    ImGui::TextColored(sFileIconCol, ContainsFiles(path) ? ICON_FA_FOLDER_OPEN : ICON_FA_FOLDER_CLOSED);
    ImGui::PopFont();
  }

  void DeleteAssetDependencies(std::filesystem::path const& path) {
    // if its a normal asset, update asset manager
    if (std::filesystem::is_regular_file(path)) {
      // remove the editor scene config file
      if (path.extension() == gSceneFileExt) {
        std::filesystem::remove(gEditorAssetsDirectory + std::string(".Scenes\\") + path.stem().string());
      }
      else {
        try {
          IGE::Assets::AssetManager& am{ IGE_ASSETMGR };
          auto guid{ am.PathToGUID(path.string()) };
          if (path.extension() == gMaterialFileExt) {
            Graphics::MaterialTable::DeleteMaterial(guid);
          }
          // for animations, remove the associated editor file as well
          else if (path.extension() == gAnimationFileExt) {
            auto const& metadata{ am.GetMetadata<IGE::Assets::AnimationAsset>(guid).metadata };
            if (metadata.contains("KeyframeEditorData")) {
              std::filesystem::remove(metadata.at("KeyframeEditorData"));
            }
          }
          //IGE_DBGLOGGER.LogInfo("DeleteFunction called on " + path.string());
          am.DeleteFunction(path.parent_path().filename().string())(guid);
        }
        catch (...) {
          //do nothing
        }
      }
    }
    // file is a directory. Recursively delete all dependencies within it
    else {
      for (auto const& file : std::filesystem::directory_iterator(path)) {
        // call this func agn but with target = targetDir/parentDir
        DeleteAssetDependencies(file.path());
      }
    }
  }

  std::string ChangeAssetPath(GUI::AssetPayload const& asset, std::filesystem::path const& targetDirectory) {
    std::string const originalPath{ asset.GetFilePath() },
      newPath{ targetDirectory.string() + "\\" + asset.mPath.filename().string() };

    // if its a normal asset file
    if (std::filesystem::is_regular_file(asset.mPath)) {
      try {
        IGE::Assets::AssetManager& am{ IGE_ASSETMGR };
        IGE::Assets::GUID const guid{ am.PathToGUID(originalPath) };

        switch (asset.mAssetType) {
        case GUI::AssetPayload::AUDIO:
          am.ChangeAssetPath<IGE::Assets::AudioAsset>(guid, newPath);
          break;
        case GUI::AssetPayload::FONT:
          am.ChangeAssetPath<IGE::Assets::FontAsset>(guid, newPath);
          break;
        case GUI::AssetPayload::MODEL:
          am.ChangeAssetPath<IGE::Assets::ModelAsset>(guid, newPath);
          break;
        case GUI::AssetPayload::PREFAB:
          am.ChangeAssetPath<IGE::Assets::PrefabAsset>(guid, newPath);
          break;
        case GUI::AssetPayload::SPRITE:
          am.ChangeAssetPath<IGE::Assets::TextureAsset>(guid, newPath);
          break;
        case GUI::AssetPayload::SHADER:
          am.ChangeAssetPath<IGE::Assets::ShaderAsset>(guid, newPath);
          break;
        case GUI::AssetPayload::ANIMATION:
          am.ChangeAssetPath<IGE::Assets::AnimationAsset>(guid, newPath);
          break;
        case GUI::AssetPayload::SCENE:
        default:
          break;
        }
      }
      catch (Debug::ExceptionBase&) {
        IGE_DBGLOGGER.LogError("Unable to get GUID of " + originalPath + ". You may wanna reimport it.");
      }
    }
    // asset is a folder:
    // recursively update the paths of every asset inside the source directory
    else {
      for (auto const& file : std::filesystem::directory_iterator(asset.mPath)) {
        // call this func agn but with target = targetDir/parentDir
        ChangeAssetPath(file.path(), newPath);
      }
    }

    //IGE_DBGLOGGER.LogInfo("Changed " + originalPath + " to " + newPath);
    return newPath;
  }

  void DisplayTempFileIcon(std::string const& ext) {
    ImGui::PushFont(GUI::GUIVault::GetStyler().GetCustomFont(GUI::CustomFonts::ROBOTO_BOLD));
    if (std::string(gSupportedModelFormats).find(ext) != std::string::npos) {
      ImGui::TextColored(sFileIconCol, ICON_FA_CUBES_STACKED);
    }
    else if (ext == gSceneFileExt) {
      ImGui::TextColored(sFileIconCol, ICON_FA_FILM);
    }
    else if (ext == gPrefabFileExt) {
      ImGui::TextColored(sFileIconCol, ICON_FA_PERSON);
    }
    else if (ext == gMaterialFileExt) {
      ImGui::TextColored(sFileIconCol, ICON_FA_GEM);
    }
    else if (ext == gSpriteFileExt || ext == ".png") {
      ImGui::TextColored(sFileIconCol, ICON_FA_IMAGE);
    }
    else if (ext == gAnimationFileExt) {
      ImGui::TextColored(sFileIconCol, ICON_FA_PERSON_RUNNING);
    }
    else if (std::string(gSupportedAudioFormats).find(ext) != std::string::npos) {
      ImGui::TextColored(sFileIconCol, ICON_FA_MUSIC);
    }
    else if (ext == gFontFileExt) {
      ImGui::TextColored(sFileIconCol, ICON_FA_FONT);
    }
    else if (ext == ".glsl") {
      ImGui::TextColored(sFileIconCol, ICON_FA_WATER);
    }
    else {
      ImGui::TextColored(sFileIconCol, ICON_FA_FILE);
    }
    ImGui::PopFont();
  }

  bool ShouldHideDirectory(std::string const& dirName) {
    return dirName == "Compiled" || (!dirName.empty() && dirName[0] == '.');
  }

  bool IsAssetFile(std::string const& fp) {
      auto fileext{ IGE::Assets::GetFileExtension(fp) };
      if (fileext == IGE::Assets::cAssetMetadataFileExtension) return false;
      return true;
  }

  void NextRowTable(const char* label) {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(label);
    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(MeshPopup::sTableCol1Width);
  }
}
