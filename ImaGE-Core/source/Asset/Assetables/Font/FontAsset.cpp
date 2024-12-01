#include "pch.h"
#include "FontAsset.h"
#include "Asset/AssetUtils.h"
#include "Asset/AssetManager.h"
#include "Asset/AssetMetadata.h"
#include "Asset/Assetables/AssetExtensions.h"

namespace IGE {

  namespace Assets {

    GUID FontAsset::Import(std::string const& fp, std::string& newFp, AssetMetadata::AssetProps& metadata) {
      std::string filename{ GetFileName(fp) };
      std::string fileext{ GetFileExtension(fp) };
      std::string inputFontPath{ cFontDirectory + filename + fileext };
      CreateDirectoryIfNotExists(cFontDirectory);

      if (!CopyFileToAssets(fp, inputFontPath)) {
        return GUID{};
      }

      newFp = inputFontPath;
      metadata["path"] = newFp;
      return GUID{ GUID::Seed{} };
    }

    void* FontAsset::Load(GUID guid) {
      // @TODO: Convert to custom file format
      std::string fp{ AssetManager::GetInstance().GUIDToPath(guid) };
      std::filesystem::path const path{ fp };
      std::string filename{ GetFileName(fp) };
      std::string fileext{ GetFileExtension(fp) };
      CreateDirectoryIfNotExists(cFontDirectory + cCompiledDirectory);

      if (cFontExtensions.find(fileext) == cFontExtensions.end())
        throw Debug::Exception<FontAsset>(Debug::LVL_ERROR, Msg("unsupported file type " + fileext));
      std::string finalfp{ cFontDirectory + cCompiledDirectory + filename + fileext };
      CopyFileToAssets(fp, finalfp);

      return reinterpret_cast<void*>(new FontAsset{ finalfp });
    }

    void FontAsset::Unload(FontAsset* ptr, GUID guid) {
      delete ptr;
    }

  }

}