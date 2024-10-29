#include <pch.h>
#include "PrefabAsset.h"
#include <Asset/AssetUtils.h>


namespace IGE::Assets {

  PrefabAsset::PrefabAsset(std::string const& fp) {

  }

  IGE::Assets::GUID PrefabAsset::Import(std::string const& fp) {
    if (!IsValidFilePath(fp)) {
      return GUID{ fp };
    }


  }

  void* PrefabAsset::Load([[maybe_unused]] GUID guid) {
    return new PrefabAsset(guid.GetSeed());
  }

  void PrefabAsset::Unload([[maybe_unused]] PrefabAsset* ptr, GUID guid) {
    delete ptr;
  }

} // namespace IGE::Asset
