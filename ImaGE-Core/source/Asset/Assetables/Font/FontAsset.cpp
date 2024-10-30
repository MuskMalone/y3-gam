#include "pch.h"
#include "FontAsset.h"

IGE::Assets::FontAsset::FontAsset(std::string const& fp) {

}

IGE::Assets::GUID IGE::Assets::FontAsset::Import(std::string const& fp, std::string& newFp, AssetMetadata::AssetProps&) {
  return IGE::Assets::GUID();
}

void* IGE::Assets::FontAsset::Load(GUID guid) {
  return nullptr;
}

void IGE::Assets::FontAsset::Unload(FontAsset* ptr, GUID guid) {

}
