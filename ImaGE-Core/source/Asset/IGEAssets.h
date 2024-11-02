#pragma once

#include "SmartPointer.h"
#include "AssetManager.h"
#include <filesystem>
//assetables
#include "Assetables/Assetables.h"
#include <Asset/AssetUtils.h>
#include <windows.h>
#include <wrl/wrappers/corewrappers.h>
#include <DirectXTex.h>

#define IGE_ASSETMGR IGE::Assets::AssetManager::GetInstance()
#define IGE_REF(type, guid) IGE::Assets::AssetManager::GetInstance().GetAsset<type>(guid)

inline void IGEAssetsRegisterTypes() {
    // COM initialization
//#if (_WIN32_WINNT >= 0x0A00 /*_WIN32_WINNT_WIN10*/)
//    Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
//    if (FAILED(initialize)) {
//        throw std::runtime_error{"Error: RoInitialize failed!" };
//    }
//#else

}
inline void IGEAssetsImportAllAssets() {
    namespace fs = std::filesystem;
    fs::path rootDir(gAssetsDirectory);
    IGE::Assets::AssetManager& am{ IGE::Assets::AssetManager::GetInstance() };
    // Check if the root path exists and is a directory
    if (!fs::exists(rootDir) || !fs::is_directory(rootDir)) {
        std::cerr << "Invalid root directory path." << std::endl;
        return;
    }
    fs::path textures { IGE::Assets::cTextureDirectory };
    fs::path audio { IGE::Assets::cAudioDirectory };
    fs::path model { IGE::Assets::cModelDirectory };
    fs::path font { IGE::Assets::cFontDirectory };
    // Iterate through the directories in the root path
    for (const auto& entry : fs::directory_iterator(rootDir)) {
        if (entry.is_directory()) {
            fs::path subDir = entry.path();
            //TODO tch: add reflection 
            // Check if we're in the "Textures" or "Audio" subdirectory
            if (IGE::Assets::IsDirectoriesEqual(subDir, textures)) {
                for (const auto& entry : fs::directory_iterator(subDir)) {
                    if (entry.is_regular_file()) {
                        am.ImportAsset<IGE::Assets::TextureAsset>(entry.path().string());
                    }
                }
                
            }
            else if (IGE::Assets::IsDirectoriesEqual(subDir, audio)) {
                for (const auto& entry : fs::directory_iterator(subDir)) {
                    if (entry.is_regular_file()) {
                        am.ImportAsset<IGE::Assets::AudioAsset>(entry.path().string());
                    }
                }
            }
            else if (IGE::Assets::IsDirectoriesEqual(subDir, model)) {
                for (const auto& entry : fs::directory_iterator(subDir)) {
                    if (entry.is_regular_file()) {
                        am.ImportAsset<IGE::Assets::ModelAsset>(entry.path().string());
                    }
                }
            }
            else if (IGE::Assets::IsDirectoriesEqual(subDir, font)) {
              for (const auto& entry : fs::directory_iterator(subDir)) {
                if (entry.is_regular_file()) {
                  am.ImportAsset<IGE::Assets::FontAsset>(entry.path().string());
                }
              }
            }
        }
    }
    //am.LoadRef<IGE::Assets::TextureAsset>("C:\\Users\\terra\\OneDrive\\Documents\\GitHub\\y3-gam\\Assets\\Textures\\foot.dds");

}
inline void IGEAssetsInitialize() {
    IGEAssetsRegisterTypes();
    //generate the files (1 time use only pls comment/delete after)
    //treats all the current asset files as fully imported already
    IGEAssetsImportAllAssets();
}
