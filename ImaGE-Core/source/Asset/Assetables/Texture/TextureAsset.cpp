#include <pch.h>
#include "TextureAsset.h"
#include <string>
#include <filesystem>
#include <Asset/AssetUtils.h>
#include "Asset/AssetManager.h"
#include "Asset/AssetMetadata.h"
#include "Asset/Assetables/AssetExtensions.h"
namespace IGE {
  namespace Assets {
    namespace {
      bool ConvertToDDS(const std::wstring& inputPath, const std::wstring& outputPath) {
        DirectX::ScratchImage image;
        DirectX::TexMetadata metadata;

        // Load a non-DDS image (PNG, JPG, BMP, etc.) using WIC
        HRESULT hr = DirectX::LoadFromWICFile(inputPath.c_str(), DirectX::WIC_FLAGS_NONE, &metadata, image);

        if (FAILED(hr)) {
#ifdef _DEBUG
          std::cerr << "Failed to load image: " << std::hex << hr << std::endl;
#endif
          return false;
        }

        // Save the image as a DDS file

// No files should be copied/created in the distribution build
#ifndef DISTRIBUTION
        hr = DirectX::SaveToDDSFile(image.GetImages(), image.GetImageCount(), metadata, DirectX::DDS_FLAGS_NONE, outputPath.c_str());

        if (FAILED(hr)) {
          std::cerr << "Failed to save as DDS: " << std::hex << hr << std::endl;
          return false;
        }
#endif           
#ifdef _DEBUG
        std::cout << "Image successfully converted to DDS: ";
#endif
        std::wcout << outputPath << std::endl;
        return true;
      }

    }
    GUID TextureAsset::Import(std::string const& fp, std::string& newFp, AssetMetadata::AssetProps& metadata) {
      // Copy the image file to the assets folder
      std::string const filename{ GetFileName(fp) };
      std::string const fileext{ GetFileExtension(fp) };
      std::string const ddsImagePath{ cTextureDirectory + cCompiledDirectory + filename + ".dds" };
      std::string const inputImagePath{ cTextureDirectory + filename + fileext };
      CreateDirectoryIfNotExists(cTextureDirectory);
      CreateDirectoryIfNotExists(cTextureDirectory + cCompiledDirectory);

      // if copy fails return an empty guid
      if (!CopyFileToAssets(fp, inputImagePath)) {
        return GUID{};
      }

      if (fileext != ".dds" && cImageExtensions.find(fileext) != cImageExtensions.end()) {
        // Convert the copied image to DDS format
        if (!ConvertToDDS(std::wstring(fp.begin(), fp.end()), std::wstring(ddsImagePath.cbegin(), ddsImagePath.cend()))) {
          throw Debug::Exception<TextureAsset>(Debug::LVL_ERROR, Msg("compilation failed to convert "
            + GetFileNameWithExtension(fp) + " to .dds"));
        }
      }
      else if (fileext == ".dds") {
        CopyFileToAssets(fp, ddsImagePath);
      }
      else {
        throw Debug::Exception<TextureAsset>(Debug::LVL_ERROR, Msg("couldnt compile " + GetFileNameWithExtension(fp) + " to dds"));
      }

      newFp = inputImagePath;
      metadata.metadata["path"] = newFp;
      return GUID{ GUID::Seed{} };
    }
    void* TextureAsset::Load([[maybe_unused]] GUID guid) {
      std::string const& fp{ AssetManager::GetInstance().GUIDToPath(guid) };
      std::string const ddsImagePath{ cTextureDirectory + cCompiledDirectory + GetFileName(fp) + ".dds" };

      return new TextureAsset(
        ddsImagePath
      );
    }
    void TextureAsset::Unload([[maybe_unused]] TextureAsset* ptr, GUID guid) {
      delete ptr;
    }
  } // namespace Assets
} // namespace IGE
