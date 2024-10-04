#pragma once

#include "SmartPointer.h"
#include "AssetManager.h"

//assetables
#include "Assetables/Texture/TextureAsset.h"
#include "Assetables/Audio/AudioAsset.h"
#include <windows.h>
#include <wrl/wrappers/corewrappers.h>
#include <DirectXTex.h>
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

    auto am{ IGE::Assets::AssetManager::GetInstance() };
    am->RegisterTypes<
        IGE::Assets::TextureAsset
    >();
}
#ifdef IGE_ASSETMGR_SAMPLE
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