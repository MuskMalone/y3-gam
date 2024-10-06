#include <pch.h>
#include "TextureAsset.h"
#include <string>
#include <filesystem>
#include <Asset/AssetUtils.h>
namespace IGE {
	namespace Assets {
		namespace {


            bool ConvertToDDS(const std::wstring& inputPath, const std::wstring& outputPath) {
                DirectX::ScratchImage image;
                DirectX::TexMetadata metadata;

                // Load a non-DDS image (PNG, JPG, BMP, etc.) using WIC
                HRESULT hr = DirectX::LoadFromWICFile(inputPath.c_str(), DirectX::WIC_FLAGS_NONE, &metadata, image);

                if (FAILED(hr)) {
                    std::cerr << "Failed to load image: " << std::hex << hr << std::endl;
                    return false;
                }

                // Save the image as a DDS file
                hr = DirectX::SaveToDDSFile(image.GetImages(), image.GetImageCount(), metadata, DirectX::DDS_FLAGS_NONE, outputPath.c_str());

                if (FAILED(hr)) {
                    std::cerr << "Failed to save as DDS: " << std::hex << hr << std::endl;
                    return false;
                }
                
                std::cout << "Image successfully converted to DDS: ";
                std::wcout <<  outputPath << std::endl;
                return true;
            }

		}
        GUID TextureAsset::Import(std::string const& fp) {
            // Copy the image file to the assets folder
            std::string filename { GetFileName(fp) };
            std::string fileext { GetFileExtension(fp) };
            std::string inputImagePath{ GetAbsolutePath(cTextureDirectory + filename + fileext) };
            std::string ddsImagePath{ GetAbsolutePath(cTextureDirectory + filename + ".dds") };
            CreateDirectoryIfNotExists(cTextureDirectory);
            if (IsDirectoriesEqual(inputImagePath, fp)) return GUID{ GetAbsolutePath(inputImagePath) };
            
            if (!CopyFileToAssets(fp, inputImagePath)) {
                return -1;
            }

            if (fileext != ".dds") {
                // Convert the copied image to DDS format
                if (!ConvertToDDS(std::wstring(inputImagePath.begin(), inputImagePath.end()), std::wstring(ddsImagePath.begin(), ddsImagePath.end()))) {
                    return -1;
                }

                // Delete the original copied image, keeping only the DDS file
                if (!DeleteFileAssets(inputImagePath)) {
                    return -1;
                }
                return GUID{ GetAbsolutePath(ddsImagePath) };
            }
            return GUID{ GetAbsolutePath(inputImagePath) };
        }
		void* TextureAsset::Load([[maybe_unused]] GUID guid) {
			return new TextureAsset(std::string(guid.GetSeed()));
		}
		void TextureAsset::Unload([[maybe_unused]] TextureAsset* ptr, GUID guid) {
			delete ptr;
		}
	}
}