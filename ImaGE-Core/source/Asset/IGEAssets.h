#pragma once

#include "SmartPointer.h"
#include "AssetManager.h"
#include <filesystem>
//assetables
#include "Assetables/Texture/TextureAsset.h"
#include "Assetables/Audio/AudioAsset.h"
#include "Assetables/Mesh/MeshAsset.h"
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
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        throw std::runtime_error{"Error: CoInitializeEx failed!"};
    }
//#endif

    IGE::Assets::AssetManager& am{ IGE::Assets::AssetManager::GetInstance() };
    am.RegisterTypes<
        IGE::Assets::TextureAsset, 
        IGE::Assets::AudioAsset,
        IGE::Assets::MeshAsset
    >();
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
                        am.ImportAsset<IGE::Assets::MeshAsset>(entry.path().string());
                    }
                }
            }
        }
    }
    //am.LoadRef<IGE::Assets::TextureAsset>("C:\\Users\\terra\\OneDrive\\Documents\\GitHub\\y3-gam\\Assets\\Textures\\foot.dds");

}
inline void IGEAssetsInitialize() {
    IGEAssetsRegisterTypes();
    IGEAssetsImportAllAssets();
}



#ifdef IGE_ASSETMGR_SAMPLE
//for testing purposes
struct Integer : public IGE::Assets::RefCounted {
    int i;
    static IGE::Assets::GUID Import(std::string const& fp) {
        //do the processing of the filepath here (eg copy to assets folder, convert to another file type etc)
        std::cout << "Integer importing " + fp << std::endl;
        return IGE::Assets::GUID{fp};
    }
    static void* Load(IGE::Assets::GUID) {
        return reinterpret_cast<void*>(new Integer());
    }
    static void Unload(Integer* ptr, IGE::Assets::GUID) {
        if (ptr) delete ptr;
    }
};
struct Float : public IGE::Assets::RefCounted {
    float f;
    static IGE::Assets::GUID Import(std::string const& fp) {
        //do the processing of the filepath here (eg copy to assets folder, convert to another file type etc)
        std::cout << "Float importing " + fp << std::endl; 
        return IGE::Assets::GUID{fp};
    }
    static void* Load(IGE::Assets::GUID) {
        return reinterpret_cast<void*>(new Float());
    }
    static void Unload(Float* ptr, IGE::Assets::GUID) {
        if (ptr) delete ptr;
    }
};

inline void AssetManagerSampleTest() {
    auto am { IGE::Assets::AssetManager::GetInstance() };        
    am->RegisterTypes<Float, Integer>();
    auto floatAssetGUID{ am->ImportAsset<Float>("../some/float/asset/here") };
    auto integerAssetGUID{ am->ImportAsset<Integer>("../other/integer/asset/here") };
    am->LoadRef<Float>(floatAssetGUID);
    am->LoadRef<Integer>(integerAssetGUID);

    IGE::Assets::Ref<Float> floatRef { am->GetAsset<Float>(floatAssetGUID) };
    IGE::Assets::Ref<Integer> integerRef { am->GetAsset<Integer>(integerAssetGUID) };
    {//testing for asset manager
        std::cout << am->GetInstanceInfo(floatRef) << std::endl;
        std::cout << am->GetInstanceInfo(integerRef) << "first" << std::endl;

        std::cout << "floatref pointer internal: " << floatRef->f << std::endl;
        std::cout << "integerref pointer internal: " << integerRef->i << std::endl << std::endl;

        IGE::Assets::Ref<Float> floatRef1 { am->GetAsset<Float>(floatAssetGUID) };
        IGE::Assets::Ref<Integer> integerRef1 { am->GetAsset<Integer>(integerAssetGUID) };

        std::cout << am->GetInstanceInfo(floatRef) << std::endl;
        std::cout << am->GetInstanceInfo(integerRef) << "second" << std::endl;

        floatRef1->f = 123.f;
        integerRef1->i = 456;
        std::cout << "floatref pointer internal after assignment: " << floatRef->f << std::endl;
        std::cout << "integerref pointer internal after assignment: " << integerRef->i << std::endl << std::endl;

        std::cout << am->GetInstanceInfo(floatRef1) << std::endl;
        std::cout << am->GetInstanceInfo(integerRef1) << "third" << std::endl;
    }
    std::cout << am->GetInstanceInfo(floatRef) << std::endl;
    std::cout << am->GetInstanceInfo(integerRef) << "fourth" << std::endl;
}
#endif