#include <pch.h>
#include "VideoAsset.h"
#include <string>
#include <filesystem>

#include <Asset/AssetUtils.h>
#include "Asset/AssetManager.h"

namespace IGE::Assets {
  GUID VideoAsset::Import(std::string const& fp, std::string& newFp, AssetMetadata::AssetProps& metadata) {
    std::string const fileName{ GetFileName(fp) };
    std::string const fileExt{ GetFileExtension(fp) };
    std::string const compiledPath{ gVideosDirectory + cCompiledDirectory + fileName + ".mpg" };

    CreateDirectoryIfNotExists(gVideosDirectory);
    CreateDirectoryIfNotExists(gVideosDirectory + cCompiledDirectory);

    // if copy fails return an empty guid
    if (!CopyFileToAssets(fp, gVideosDirectory)) {
      return GUID{};
    }

    // do nothing since this function is called after the
    // video is imported into the compiled directory

    newFp = std::string(gVideosDirectory) + fileName + fileExt;
    metadata.metadata["path"] = newFp;
    return GUID{ GUID::Seed{} };
  }

  void* VideoAsset::Load(GUID guid) {
    std::string const& fp{ AssetManager::GetInstance().GUIDToPath(guid) };
    std::string const compiledVideoPath{ gVideosDirectory + cCompiledDirectory + GetFileName(fp) + ".mpg"};

    return new VideoAsset(
      compiledVideoPath
    );
  }

  void VideoAsset::Unload(VideoAsset* ptr, GUID guid) {
    delete ptr;
  }
} // namespace IGE::Assets
