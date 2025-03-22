#include "pch.h"
#include "FontAsset.h"
#include "Asset/AssetUtils.h"
#include "Asset/AssetManager.h"
#include "Asset/AssetMetadata.h"
#include "Asset/Assetables/AssetExtensions.h"

namespace IGE {

  namespace Assets {

    GUID FontAsset::Import(std::string const& fp, std::string& newFp, AssetMetadata::AssetProps& metadata) {
      std::string const inputFontPath{ cFontDirectory + GetFileNameWithExtension(fp) };
      CreateDirectoryIfNotExists(cFontDirectory);
      CreateDirectoryIfNotExists(cFontDirectory + cCompiledDirectory);

      if (!CopyFileToAssets(fp, inputFontPath)) {
        return GUID{};
      }

      if (cFontExtensions.find(GetFileExtension(fp)) == cFontExtensions.end())
        throw Debug::Exception<FontAsset>(Debug::LVL_ERROR, Msg("unsupported file type " + GetFileNameWithExtension(fp)));

      newFp = inputFontPath;
      metadata.metadata["path"] = newFp;
      return GUID{ GUID::Seed{} };
    }

    void* FontAsset::Load(GUID guid) {
      // @TODO: Convert to custom file format
      std::string const& fp{ AssetManager::GetInstance().GUIDToPath(guid) };
      std::string const finalfp{ cFontDirectory + cCompiledDirectory + GetFileNameWithExtension(fp) };

      CopyFileToAssets(fp, finalfp);

      return reinterpret_cast<void*>(new FontAsset{ finalfp });
    }

    void FontAsset::Unload(FontAsset* ptr, GUID guid) {
      delete ptr;
    }

  }

}