/*!*********************************************************************
\file   AssetPayload.h
\author chengen.lau\@digipen.edu
\date   16-September-2024
\brief  Definition of struct AssetPayload, which is used to handle
        drag and dropped payload data for assets. It defines functions
        to help with extracting the relevant asset type and path. This
        is mainly used by the Viewport class.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
#include <filesystem>
#include <string>
#include <Globals.h>
#include <memory>
#include <Asset/Assetables/AssetExtensions.h>

namespace GUI
{
  struct AssetPayload
  {
    enum AssetType
    {
      NONE,
      SPRITE,
      PREFAB,
      SCENE,
      MODEL,
      MATERIAL,
      AUDIO,
      FONT,
      SHADER
    };

    static constexpr char sAssetDragDropPayload[] = "ASSET";

    AssetPayload(std::filesystem::path const& path) : mPath{ path }, mAssetType{ AssetType::NONE } {
      std::string const fileExt{ path.extension().string() };
      if (fileExt == gSceneFileExt) {
        mAssetType = AssetType::SCENE;
      }
      else if (fileExt == gPrefabFileExt) {
        mAssetType = AssetType::PREFAB;
      }
      else if (IGE::Assets::cMeshExtensions.find(fileExt) != IGE::Assets::cMeshExtensions.end()) {
        mAssetType = AssetType::MODEL;
      }
      else if (fileExt == gMaterialFileExt) {
        mAssetType = AssetType::MATERIAL;
      }
      else if (IGE::Assets::cAudioExtensions.find(fileExt) != IGE::Assets::cAudioExtensions.end()) {
          mAssetType = AssetType::AUDIO;
      }
      else if (fileExt == gFontFileExt) {
        mAssetType = AssetType::FONT;
      }
      else if (IGE::Assets::cShaderExtensions.find(fileExt) != IGE::Assets::cShaderExtensions.end()){
        mAssetType = AssetType::SHADER;
      }
      else if (IGE::Assets::cImageExtensions.find(fileExt) != IGE::Assets::cImageExtensions.end()) {
        mAssetType = AssetType::SPRITE;
      }
    }

    std::string GetFileName() const { return mPath.stem().string(); }
    std::string GetFileNameWithExt() const { return mPath.filename().string(); }
    std::string GetFilePath() const { return mPath.relative_path().string(); }

    std::filesystem::path mPath;
    AssetType mAssetType;
  };

} // namespace GUI
