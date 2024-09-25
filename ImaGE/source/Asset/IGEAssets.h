#pragma once
#include <pch.h>

#include "SmartPointer.h"
#include "AssetManager.h"

//for testing purposes
struct Integer : public IGE::Assets::RefCounted {
    int i;
    static void* Load(IGE::Assets::GUID) {
        return reinterpret_cast<void*>(new Integer());
    }
    static void Unload(Integer* ptr, IGE::Assets::GUID) {
        if (ptr) delete ptr;
    }
};

struct Float : public IGE::Assets::RefCounted {
    float f;
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
        std::cout << am->GetInstanceInfo(integerRef) << std::endl;

        std::cout << "floatref pointer internal: " << floatRef->f << std::endl;
        std::cout << "integerref pointer internal: " << integerRef->i << std::endl << std::endl;

        IGE::Assets::Ref<Float> floatRef1 { am->GetAsset<Float>(floatAssetGUID) };
        IGE::Assets::Ref<Integer> integerRef1 { am->GetAsset<Integer>(integerAssetGUID) };

        std::cout << am->GetInstanceInfo(floatRef1) << std::endl;
        std::cout << am->GetInstanceInfo(integerRef1) << std::endl;

        floatRef1->f = 123.f;
        integerRef1->i = 456;
        std::cout << "floatref pointer internal after assignment: " << floatRef->f << std::endl;
        std::cout << "integerref pointer internal after assignment: " << integerRef->i << std::endl << std::endl;

        std::cout << am->GetInstanceInfo(floatRef1) << std::endl;
        std::cout << am->GetInstanceInfo(integerRef1) << std::endl;
    }
    std::cout << am->GetInstanceInfo(floatRef) << std::endl;
    std::cout << am->GetInstanceInfo(integerRef) << std::endl;
}