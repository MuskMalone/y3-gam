#include <pch.h>
#ifndef IMGUI_DISABLE
#include "AssetBrowser.h"
#include <imgui/imgui.h>
#include <Events/EventManager.h>
#include <GUI/Helpers/AssetHelpers.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
#include <GUI/Styles/FontAwesome6Icons.h>
#include "GUI/GUIManager.h"
#include <Graphics/AssetIO/IMSH.h>

namespace Helper
{
  /*!*********************************************************************
  \brief
    Checks if the given directory contains more directories
  \param dirEntry
    The path of the file
  \return
    True if it contains at least 1 directory and false otherwise
  ************************************************************************/
  bool ContainsDirectories(std::filesystem::path const& dirEntry);
}

namespace GUI
{

  AssetBrowser::AssetBrowser(std::string const& name) : GUIWindow(name),
    mCurrentDir{ gAssetsDirectory }, mRightClickedDir{},
    mSelectedAsset{}, mDirMenuPopup{ false }, mAssetMenuPopup{ false }, mDisableSceneChange{ false }
  {
    SUBSCRIBE_CLASS_FUNC(Events::EventType::SCENE_STATE_CHANGE, &AssetBrowser::HandleEvent, this);
    SUBSCRIBE_CLASS_FUNC(Events::EventType::ADD_FILES, &AssetBrowser::HandleEvent, this);
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
  }

  void AssetBrowser::MenuBar()
  {
    ImGui::BeginMenuBar();
    bool const isSearching{ !mSearchQuery.empty() };
    float const wWidth{ ImGui::GetWindowWidth() };
    
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 70));
    if (ImGui::Button(ICON_FA_PLUS " Add")) {
      auto const files{ AssetHelpers::SelectFilesFromExplorer("Add Files") };

      if (!files.empty()) {
        AddAssets(files);
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
    ImGui::InputTextWithHint("##SearchBar", ICON_FA_MAGNIFYING_GLASS " Search Assets", &mSearchQuery, ImGuiInputTextFlags_AutoSelectAll);
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
      if (mSearchQuery.empty()) {
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
      if (fileName.find(mSearchQuery) == std::string::npos) { continue; }

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
      if (mDisableSceneChange) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
      }
      else {
        std::string const pathStr{ draggedAsset.relative_path().string() };
        ImGui::SetDragDropPayload(sAssetDragDropPayload, pathStr.data(), pathStr.size() + 1, ImGuiCond_Once);
      }
      
      ImGui::Text(draggedAsset.filename().string().c_str());

      if (mDisableSceneChange) {
        ImGui::PopStyleColor();
      }

      ImGui::EndDragDropSource();
    }
  }

  EVENT_CALLBACK_DEF(AssetBrowser, HandleEvent)
  {
    switch (event->GetCategory())
    {
    case Events::EventType::ADD_FILES:
      AddAssets(std::static_pointer_cast<Events::AddFilesFromExplorerEvent>(event)->mPaths);
      break;
    case Events::EventType::SCENE_STATE_CHANGE:
    {
      switch (CAST_TO_EVENT(Events::SceneStateChange)->mNewState)
      {
      case Events::SceneStateChange::NEW:
      case Events::SceneStateChange::CHANGED:
      case Events::SceneStateChange::STOPPED:
        mDisableSceneChange = false;
        break;
      case Events::SceneStateChange::STARTED:
      case Events::SceneStateChange::PAUSED:
        mDisableSceneChange = true;
        break;
      default: break;
      }
    }
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

  void AssetBrowser::AddAssets(std::vector<std::string> const& files)
  {
    for (std::string const& file : files) {
      // @TODO: SHOULD BE DONE BY ASSET MANAGER
      std::filesystem::path const path{ file };
      if (std::string(gSupportedModelFormats).find(path.extension().string()) != std::string::npos) {
        Graphics::AssetIO::IMSH imsh{ file };
        imsh.WriteToBinFile(path.stem().string(), file);
        continue;
      }

      std::filesystem::copy(file, mCurrentDir);
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
      if (ImGui::Selectable("Open")) {
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

      if (ImGui::Selectable("Delete")) {
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
      if (ImGui::Button("No"))
      {
        ImGui::CloseCurrentPopup();
      }
      ImGui::PopStyleColor();

      ImGui::SameLine();
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.f));
      if (ImGui::Button("Yes"))
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
        std::remove(mSelectedAsset.relative_path().string().c_str());
        ImGui::CloseCurrentPopup();
      }
      ImGui::PopStyleColor();

      ImGui::EndPopup();
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

#endif  // IMGUI_DISABLE
