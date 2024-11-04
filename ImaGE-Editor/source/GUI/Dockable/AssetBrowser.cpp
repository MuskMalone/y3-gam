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
#include <Events/EventManager.h>
#include <GUI/Helpers/AssetHelpers.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <GUI/Styles/FontAwesome6Icons.h>
#include "GUI/GUIManager.h"
#include <Graphics/AssetIO/IMSH.h>
#include <GUI/Helpers/AssetPayload.h>

namespace MeshPopup {
  static std::vector<std::string> sModelsToImport;
  static std::string sMeshPopupInput, sMeshName;
  static bool sOpenMeshPopup{ false }, sMeshOverwriteWarning{ false };
}

namespace
{
  static std::string sSearchQuery, sLowerSearchQuery;  

  /*!*********************************************************************
  \brief
    Checks if the given directory contains more directories
  \param dirEntry
    The path of the file
  \return
    True if it contains at least 1 directory and false otherwise
  ************************************************************************/
  bool ContainsDirectories(std::filesystem::path const& dirEntry);

  /*!*********************************************************************
  \brief
    Converts a string to lower case
  \param str
    The string
  \return
    The string in lower case
  ************************************************************************/
  std::string ToLower(std::string const& str);
}

namespace GUI
{

  AssetBrowser::AssetBrowser(const char* name) : GUIWindow(name),
    mCurrentDir{ gAssetsDirectory }, mRightClickedDir{},
    mSelectedAsset{}, mDirMenuPopup{ false }, mAssetMenuPopup{ false }, mDisableSceneChange{ false }, mDisablePrefabSpawn{ true }
  {
    SUBSCRIBE_CLASS_FUNC(Events::EventType::SCENE_STATE_CHANGE, &AssetBrowser::SceneStateChanged, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::ADD_FILES, &AssetBrowser::FilesImported, this);
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
      if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        mCurrentDir = gAssetsDirectory;
      }
      else if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
        mRightClickedDir = gAssetsDirectory;
        mDirMenuPopup = true;
      }

      for (auto const& file : std::filesystem::directory_iterator(gAssetsDirectory))
      {
        if (!file.is_directory()) { continue; }

        RecurseDownDirectory(file);
      }

      ImGui::TreePop();
    }

    if (mDirMenuPopup) {
      ImGui::OpenPopup("DirectoryMenu");
      mDirMenuPopup = false;
    }
    DirectoryMenuPopup();
  }


  void AssetBrowser::ContentViewer()
  {
    if (mCurrentDir.empty()) { return; }

    float const regionAvailX{ 0.8f * ImGui::GetContentRegionAvail().x };
    int const assetsPerRow{ regionAvailX > sMaxAssetSize * 10.f ? 10 : regionAvailX < sMaxAssetSize * 4.f ? 3 : 6 };
    float const sizePerAsset{ regionAvailX / static_cast<float>(assetsPerRow) };
    float const imgSize{ sizePerAsset > sMaxAssetSize ? sMaxAssetSize : sizePerAsset };

    unsigned const maxChars{ static_cast<unsigned>(imgSize * 0.9f / ImGui::CalcTextSize("L").x) };
    if (ImGui::BeginTable("DirectoryTable", assetsPerRow, ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY))
    {
      if (sSearchQuery.empty()) {
        DisplayDirectory(imgSize, maxChars);
      }
      else {
        DisplaySearchResults(imgSize, maxChars);
      }
      ImGui::EndTable();
    }

    if (mAssetMenuPopup) {
      ImGui::OpenPopup("AssetsMenu");
      mAssetMenuPopup = false;
    }
    AssetMenuPopup();
  }

  void AssetBrowser::DisplayDirectory(float imgSize, unsigned maxChars)
  {
    for (auto const& file : std::filesystem::directory_iterator(mCurrentDir)) {
      if (file.is_directory()) { continue; }

      ImGui::TableNextColumn();
      std::string const fileName{ file.path().filename().string() };
      bool const exceed{ fileName.size() > maxChars };

      // asset icon + input
      ImGui::ImageButton(0, ImVec2(imgSize, imgSize));
      CheckInput(file);

      // display file name below
      ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 0.05f * imgSize);
      ImGui::Text((exceed ? fileName.substr(0, maxChars) : fileName).c_str());
      if (exceed) {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 0.05f * imgSize);
        std::string const secondRow{ fileName.substr(maxChars) };
        ImGui::Text((secondRow.size() > maxChars ? secondRow.substr(0, maxChars - 2) + "..." : secondRow).c_str());
      }
      ImGui::NewLine();
    }
  }

  void AssetBrowser::DisplaySearchResults(float imgSize, unsigned maxChars)
  {
    for (auto const& file : std::filesystem::recursive_directory_iterator(gAssetsDirectory))
    {
      if (file.is_directory()) { continue; }

      std::string const fileName{ file.path().filename().string() };
      if (ToLower(fileName).find(sLowerSearchQuery) == std::string::npos) { continue; }

      ImGui::TableNextColumn();
      bool const exceed{ fileName.size() > maxChars };

      // asset icon + input
      ImGui::ImageButton(0, ImVec2(imgSize, imgSize));
      CheckInput(file);

      // display file name below
      ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 0.05f * imgSize);
      ImGui::Text((exceed ? fileName.substr(0, maxChars) : fileName).c_str());
      if (exceed) {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 0.05f * imgSize);
        std::string const secondRow{ fileName.substr(maxChars) };
        ImGui::Text((secondRow.size() > maxChars ? secondRow.substr(0, maxChars - 2) + "..." : secondRow).c_str());
      }
      ImGui::NewLine();
    }
  }

  void AssetBrowser::CheckInput(std::filesystem::path const& path)
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
        AssetHelpers::OpenFileWithDefaultProgram(path.string());
      }
      else {
        draggedAsset = path;
      }
    }
    else if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
      ImGui::SetTooltip(path.filename().string().c_str());
    }
    
    if (ImGui::BeginDragDropSource()) {
      bool const canSpawnPfb{ mDisablePrefabSpawn && draggedAsset.extension() == gPrefabFileExt };
      if (mDisableSceneChange || canSpawnPfb) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
      }
      else {
        std::string const pathStr{ draggedAsset.relative_path().string() };
        ImGui::SetDragDropPayload(AssetPayload::sAssetDragDropPayload, pathStr.data(), pathStr.size() + 1, ImGuiCond_Once);
      }
      
      ImGui::Text(draggedAsset.filename().string().c_str());

      if (mDisableSceneChange || canSpawnPfb) {
        ImGui::PopStyleColor();
      }

      ImGui::EndDragDropSource();
    }
  }

  EVENT_CALLBACK_DEF(AssetBrowser, SceneStateChanged)
  {
    switch (CAST_TO_EVENT(Events::SceneStateChange)->mNewState)
    {
    case Events::SceneStateChange::NEW:
    case Events::SceneStateChange::CHANGED:
    case Events::SceneStateChange::STOPPED:
      mDisableSceneChange = mDisablePrefabSpawn = false;
      break;
    case Events::SceneStateChange::STARTED:
    case Events::SceneStateChange::PAUSED:
      mDisableSceneChange = true;
      break;
    default: break;
    }
  }

  void OpenMeshPopup() {
    MeshPopup::sOpenMeshPopup = true;
    MeshPopup::sMeshName = MeshPopup::sMeshPopupInput = std::filesystem::path(MeshPopup::sModelsToImport.back()).filename().string();
    MeshPopup::sMeshOverwriteWarning = std::filesystem::exists(gMeshOutputDir + MeshPopup::sMeshPopupInput);
  }

  EVENT_CALLBACK_DEF(AssetBrowser, FilesImported) {
    auto const& files{ CAST_TO_EVENT(Events::AddFilesFromExplorerEvent)->mPaths };
    std::vector<std::string> filesToRegister{};

    for (std::string const& file : files) {
      std::filesystem::path const path{ file };
      if (std::string(gSupportedModelFormats).find(path.extension().string()) == std::string::npos) {
        filesToRegister.emplace_back(file);
      }
      else {
        MeshPopup::sModelsToImport.emplace_back(file);
      }
    }

    if (!filesToRegister.empty()) {
      QUEUE_EVENT(Events::RegisterAssetsEvent, std::move(filesToRegister));
    }

    if (!MeshPopup::sModelsToImport.empty()) {
      OpenMeshPopup();
    }
  }

  void AssetBrowser::RecurseDownDirectory(std::filesystem::path const& path)
  {
    std::string const fileName{ path.filename().string() };
    bool const hasDirectories{ ContainsDirectories(path) };
    ImGuiTreeNodeFlags flag{ ImGuiTreeNodeFlags_SpanFullWidth };
    flag |= hasDirectories ? ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick : ImGuiTreeNodeFlags_Leaf;
    if (path == mCurrentDir) { flag |= ImGuiTreeNodeFlags_Selected; }

    if (ImGui::TreeNodeEx(fileName.c_str(), flag))
    {
      if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        mCurrentDir = path;
      }
      else if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
        mRightClickedDir = path;
        mDirMenuPopup = true;
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

  void AssetBrowser::DirectoryMenuPopup() const
  {
    static bool deletePopup{ false };
    if (ImGui::BeginPopup("DirectoryMenu"))
    {
      if (ImGui::Selectable("Open in File Explorer")) {
        AssetHelpers::OpenDirectoryInExplorer(mRightClickedDir);
      }

      ImGui::EndPopup();
    }
  }

  void AssetBrowser::AssetMenuPopup()
  {
    static bool deletePopup{ false };
    if (ImGui::BeginPopup("AssetsMenu"))
    {
      if (ImGui::Selectable("Open##AssetMenu")) {
        AssetHelpers::OpenFileWithDefaultProgram(mSelectedAsset.string());
      }

      if (ImGui::Selectable("Open in File Explorer")) {
        AssetHelpers::OpenFileInExplorer(mSelectedAsset);
      }

      // only enabled for prefabs
      if (mSelectedAsset.extension().string() == ".pfb")
      {
        ImGui::BeginDisabled(mDisableSceneChange);
        if (ImGui::Selectable("Edit Prefab"))
        {
          QUEUE_EVENT(Events::EditPrefabEvent, mSelectedAsset.stem().string(),
            mSelectedAsset.relative_path().string());
          mDisableSceneChange = true;
        }
        ImGui::EndDisabled();
      }

      if (ImGui::Selectable("Delete##AssetMenu")) {
        deletePopup = true;
      }

      ImGui::EndPopup();
    }

    if (deletePopup) {
      ImGui::OpenPopup("Confirm Delete");
      deletePopup = false;
    }
    ConfirmDeletePopup();
  }

  void AssetBrowser::ConfirmDeletePopup() const
  {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Confirm Delete", NULL, ImGuiWindowFlags_AlwaysAutoResize))
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
        // @TODO: UPDATE ASSET MANAGER ACCORDINGLY
        //Assets::AssetManager const& am{ Assets::AssetManager::GetInstance() };
        //// if prefab deleted
        //if (asset.extension() == am.PrefabFileExt)
        //{
        //  Events::EventManager::GetInstance().Dispatch(Events::DeletePrefabEvent(asset.stem().string()));
        //}
        //else
        //{
        //  Assets::AssetType type{ Assets::AssetType::NONE };
        //  if (asset.extension() == am.SceneFileExt)
        //    type = Assets::AssetType::SCENE;
        //  else if (asset.extension() == am.ImageFileExt)
        //    type = Assets::AssetType::IMAGES;
        //  else if (asset.extension() == am.AudioFileExt)
        //    type = Assets::AssetType::AUDIO;
        //  else if (asset.extension() == am.FontFileExt)
        //    type = Assets::AssetType::FONTS;
        //  else if (asset.extension() == am.ShaderFileExt)
        //    type = Assets::AssetType::SHADERS;

        //  Events::EventManager::GetInstance().Dispatch(Events::DeleteAssetEvent(type, asset.stem().string()));
        //}
        // should be done w events i think

        auto path{ mSelectedAsset.relative_path().string() };
        try {
            auto guid{ IGE_ASSETMGR.PathToGUID(mSelectedAsset.relative_path().string()) };
            IGE_ASSETMGR.DeleteFunction(mSelectedAsset.parent_path().filename().string())(guid);
        }
        catch (...) {
            //do nothing
        }
        std::remove(mSelectedAsset.relative_path().string().c_str());

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

    ImGui::Text("Import mesh to:");
    float const offset{ ImGui::GetTextLineHeight() * 0.5f };
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Assets\\Models\\");
    ImGui::SameLine();
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
    ImGui::NewLine();

    float const contentRegionX{ ImGui::GetWindowContentRegionMax().x };
    ImGui::SetCursorPosX(0.5f * (contentRegionX - ImGui::CalcTextSize("Static Mesh 123").x));
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Static Mesh");
    ImGui::SameLine();
    ImGui::Checkbox("##StaticMesh", &Graphics::AssetIO::IMSH::sStaticMeshConversion);
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("Combine all sub-meshes into a single mesh entity");
    }

    ImGui::NewLine();
    ImGui::SetCursorPosX(0.5f * (contentRegionX - ImGui::CalcTextSize("Cancel Create ").x));
    if (ImGui::Button("Cancel##Mesh") || ImGui::IsKeyPressed(ImGuiKey_Escape)) {
      close = true;
      ImGui::CloseCurrentPopup();
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
        IGE_ASSETMGR.ImportAsset<IGE::Assets::ModelAsset>(MeshPopup::sModelsToImport.back());

        close = true;
        ImGui::CloseCurrentPopup();
      }
    }

    if (close) {
      blankWarning = MeshPopup::sMeshOverwriteWarning = false;
      // erase the last element
      MeshPopup::sModelsToImport.erase(MeshPopup::sModelsToImport.begin() + MeshPopup::sModelsToImport.size() - 1);
      // if there are still more to import, run the popup again
      if (!MeshPopup::sModelsToImport.empty()) {
        OpenMeshPopup();
      }
      else {
        MeshPopup::sMeshPopupInput.clear();
      }
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::EndPopup();
  }

} // namespace GUI


namespace
{
  bool ContainsDirectories(std::filesystem::path const& dirEntry)
  {
    for (auto const& file : std::filesystem::directory_iterator(dirEntry)) {
      if (file.is_directory()) { return true; }
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
}
