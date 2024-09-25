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