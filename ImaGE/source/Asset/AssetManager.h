#pragma once
#include <pch.h>
#include "Core/GUID.h"
#include "SmartPointer.h"
#include <rttr/type.h>
#include <any>
#include <filesystem>
namespace IGE {
	namespace Assets {
        class AssetManager
        {
        private:
            using RefAny = std::any;
            using TypeKey = std::uint64_t;
            using TypeAssetKey = std::uint64_t;
            template <typename T>
            auto GetTypeName(){ return rttr::type::get<T>().get_name().to_string(); }
        public:
            //-------------------------------------------------------------------------
            AssetManager()
            {
                ////
                //// Initialize our memory manager of instance infos
                ////
                //for (int i = 0, end = (int)mInfoBuffer.size() - 1; i != end; ++i)
                //{
                //    mInfoBuffer[i].mpData = &mInfoBuffer[i + 1];
                //}
                //mInfoBuffer[mInfoBuffer.size() - 1].mpData = nullptr;
                //mpInfoBufferEmptyHead = mInfoBuffer.data();
            }

            //-------------------------------------------------------------------------
            template< typename...T_ARGS >
            void RegisterTypes()
            {
                //
                // Insert all the types into the hash table
                //
                ([&]< typename T >(T*)
                {
                    auto name = GetTypeName<T>();
                    auto typeguid = TypeGUID{ name };
                    mRegisteredTypes.emplace
                    (
                        typeguid,
                        [](resource::AssetManager& Mgr, Core::GUID GUID) { return type<T>::Load(Mgr, GUID); },
                        [](void* pData, resource::AssetManager& Mgr, Core::GUID GUID) {
                            type<T>::Destroy(*reinterpret_cast<T*>(pData), Mgr, GUID);
                        },
                        name
                    );
                }(reinterpret_cast<T_ARGS*>(0))
                    , ...
                    );
            }
            template <typename T>
            GUID ImportAsset(std::filesystem::path const& filepath) { //
                //get the absolute file path so that there cant be any duplicates
                GUID guid{ std::filesystem::absolute(filepath).string() };
                TypeGUID typeguid{ GetTypeName<T>() };
                TypeAssetKey key{ typeguid ^ guid };
                if (mAssetRefs.find(key) == mAssetRefs.end()) {
                    Ref<T> ref{};
                    ref.mInstance = {
                        PartialRef{ guid, nullptr },
                        typeguid
                    };
                    ref.mUInfo = {
                        mRegisteredTypes.at(TypeGUID)
                    };
                    ref.mInfo.guid = guid;
                    ref.mInfo.typeGUID = typeguid;
                    ref.mInfo.filepath = std::filesystem::absolute(filepath).string();

                    mAssetRefs.emplace(key, ref);
                    return GUID;
                }
                else {
                    Ref<T> ref { std::any_cast<Ref<T>>(mAssetRefs.at(key)) };
                    return ref.mInstance.partialRef.guid;
                }
            }
            template <typename T>
            GUID DeleteAsset(GUID const& guid) {
                TypeGUID typeguid{ GetTypeName<T>() };
                TypeAssetKey key{ typeguid ^ guid };
                Ref<T> ref { std::any_cast<Ref<T>>(mAssetRefs.at(key)) };
                std::filesystem::remove(ref.mInfo.filepath);
                ref.mUInfo.destroyFunc();
                mAssetRefs.erase(key);
                //ref destructor will take care of unloading the asset
            }

            //-------------------------------------------------------------------------
            template< typename T >
            T* GetAsset(Ref<T>& ref)
            {
                //// If we already have the resource return now
                if (ref.partialRef.IsPointer()) return reinterpret_cast<T*>(ref.partialRef.pointer);
                else return nullptr; //should i throw here?

                //std::uint64_t HashID = ref.partialRef.guid ^ ref.typeGUID;
                //if (auto Entry = mResourceInstance.find(HashID); Entry != mResourceInstance.end())
                //{
                //    auto& E = *Entry->second;
                //    E.mRefCount++;
                //    return reinterpret_cast<T*>(ref.partialRef.pointer = E.mpData);
                //}

                //T* pRSC = type<T>::Load(*this, ref.partialRef.guid);
                //// If the user return nulls it must mean that it failed to load so we could return a temporary resource of the right type
                //if (pRSC == nullptr) return nullptr;

                //FullInstanceInfoAlloc(pRSC, ref.partialRef.guid, ref.typeGUID);

                //return reinterpret_cast<T*>(ref.partialRef.pointer = pRSC);

            }

            //-------------------------------------------------------------------------
            template<typename T>
            T* GetAsset(GUID const& guid)
            {
                TypeGUID typeguid{ GetTypeName<T>() };
                TypeAssetKey key{ typeguid ^ guid };
                if (mAssetRefs.find(key) != mAssetRefs.end())
                    return GetAsset<T>(std::any_cast<Ref<T>>(mAssetRefs.at(key)));
                else throw std::exception(); // should be a specialized exception
                //// If we already have the resource return now
                //if (uref.partialRef.IsPointer()) return reinterpret_cast<T*>(uref.partialRef.pointer);

                //std::uint64_t HashID = uref.partialRef.guid ^ uref.typeGUID;
                ////if (auto Entry = mResourceInstance.find(HashID); Entry != mResourceInstance.end())
                ////{
                ////    auto& E = *Entry->second;
                ////    E.mRefCount++;
                ////    uref.partialRef.pointer = E.mpData;
                ////    return uref.partialRef.pointer;
                ////}

                //auto UniversalType = mRegisteredTypes.find(uref.typeGUID);
                //assert(UniversalType != mRegisteredTypes.end()); // Type was not registered

                //void* pRSC = UniversalType->second.loadFunc(*this, uref.partialRef.guid);
                //// If the user return nulls it must mean that it failed to load so we could return a temporary resource of the right type
                //if (pRSC == nullptr) return nullptr;

                //FullInstanceInfoAlloc(pRSC, uref.partialRef.guid, uref.typeGUID);

                //return uref.partialRef.pointer = pRSC;
            }

            //-------------------------------------------------------------------------
            template< typename T >
            void UnloadRef(Ref<T>& ref){
                if (ref.mInstance.partialRef.IsPointer()) {
                    ref.mUInfo.destroyFunc(ref.mInstance.partialRef.pointer);
                    ref.mInstance.partialRef.pointer = nullptr;
                }
                //if (false == ref.partialRef.IsPointer() || ref.partialRef.guid == 0) return;

                //auto S = mResourceInstanceRelease.find(reinterpret_cast<std::uint64_t>(ref.partialRef.pointer));
                //assert(S != mResourceInstanceRelease.end());

                //auto& R = *S->second;
                //R.mRefCount--;
                //auto OriginalGuid = R.guid;
                //assert(R.typeGUID == ref.typeGUID);
                //assert(R.mpData == ref.partialRef.pointer);

                ////
                //// If this is the last reference release the resource
                ////
                //if (R.mRefCount == 0)
                //{
                //    type<T>::Destroy(*reinterpret_cast<T*>(ref.partialRef.pointer), *this, R.guid);
                //    FullInstanceInfoRelease(R);
                //}

                //ref.partialRef.guid = OriginalGuid;
            }

            template< typename T >
            void UnloadRef(GUID guid) {
                //if (false == ref.partialRef.IsPointer() || ref.partialRef.guid == 0) return;
                TypeGUID typeguid{ GetTypeName<T>() };
                TypeAssetKey key{ typeguid ^ guid };
                if (mAssetRefs.find(key) != mAssetRefs.end())
                    return UnloadRef<T>(std::any_cast<Ref<T>>(mAssetRefs.at(key)));
                //auto S = mResourceInstanceRelease.find(reinterpret_cast<std::uint64_t>(ref.partialRef.pointer));
                //assert(S != mResourceInstanceRelease.end());

                //auto& R = *S->second;
                //R.mRefCount--;
                //auto OriginalGuid = R.guid;
                //assert(R.typeGUID == ref.typeGUID);
                //assert(R.mpData == ref.partialRef.pointer);

                ////
                //// If this is the last reference release the resource
                ////
                //if (R.mRefCount == 0)
                //{
                //    type<T>::Destroy(*reinterpret_cast<T*>(ref.partialRef.pointer), *this, R.guid);
                //    FullInstanceInfoRelease(R);
                //}

                //ref.partialRef.guid = OriginalGuid;
            }

            //-------------------------------------------------------------------------
            //void ReleaseRef(UniversalRef& uref)
            //{
            //    if (false == uref.partialRef.IsPointer() || uref.partialRef.guid == 0) return;

            //    //auto S = mResourceInstanceRelease.find(reinterpret_cast<std::uint64_t>(uref.partialRef.pointer));
            //    //assert(S != mResourceInstanceRelease.end());

            //    auto& R = *S->second;
            //    R.mRefCount--;
            //    auto OriginalGuid = R.guid;
            //    assert(uref.typeGUID == R.typeGUID);
            //    assert(R.mpData == uref.partialRef.pointer);

            //    //
            //    // If this is the last reference release the resource
            //    //
            //    if (R.mRefCount == 0)
            //    {
            //        auto UniversalType = mRegisteredTypes.find(uref.typeGUID);
            //        assert(UniversalType != mRegisteredTypes.end()); // Type was not registered

            //        UniversalType->second.mpDestroyFunc(uref.partialRef.pointer, *this, R.guid);

            //        FullInstanceInfoRelease(R);
            //    }

            //    uref.partialRef.guid = OriginalGuid;
            //}

            template< typename T >
            GUID getInstanceGuid(const Ref<T>& ref) const
            {
                return ref.mInstance.partialRef.guid;
                //if (false == ref.GetUniversalRef().IsPointer()) return ref.GetUniversalRef().partialRef.guid;

                //auto S = mResourceInstanceRelease.find(reinterpret_cast<std::uint64_t>(ref.partialRef.pointer));
                //assert(S != mResourceInstanceRelease.end());

                //return ref.GetUniversalRef().partialRef.guid;
            }

            //// When serializing resources of displaying them in the editor you may want to show the GUID rather than the pointer
            //// When reference holds the pointer rather than the GUID we must find the actual GUID to return to the user
            //GUID getInstanceGuid(const UniversalRef& uref) const
            //{
            //    //if (false == uref.partialRef.IsPointer()) return uref.partialRef.guid;

            //    //auto S = mResourceInstanceRelease.find(reinterpret_cast<std::uint64_t>(uref.partialRef.pointer));
            //    //assert(S != mResourceInstanceRelease.end());

            //    //return S->second->guid;
            //}

            //template< typename T >
            //void CloneRef(Ref<T>& Dest, const Ref<T>& ref) noexcept
            //{
            //    //if (ref.partialRef.IsPointer())
            //    //{
            //    //    if (Dest.partialRef.IsPointer())
            //    //    {
            //    //        if (Dest.partialRef.pointer == ref.partialRef.pointer) return;
            //    //        ReleaseRef(Dest);
            //    //    }

            //    //    auto S = mResourceInstanceRelease.find(reinterpret_cast<std::uint64_t>(ref.partialRef.pointer));
            //    //    assert(S != mResourceInstanceRelease.end());

            //    //    S->second->mRefCount++;
            //    //}
            //    //else
            //    //{
            //    //    if (Dest.partialRef.IsPointer()) ReleaseRef(Dest);
            //    //}
            //    //Dest.partialRef = ref.partialRef;
            //}

            //void CloneRef(UniversalRef& Dest, const UniversalRef& URef) noexcept
            //{
            //    //if (URef.partialRef.IsPointer())
            //    //{
            //    //    if (Dest.partialRef.IsPointer())
            //    //    {
            //    //        if (Dest.partialRef.pointer == URef.partialRef.pointer) return;
            //    //        ReleaseRef(Dest);
            //    //    }

            //    //    auto S = mResourceInstanceRelease.find(reinterpret_cast<std::uint64_t>(URef.partialRef.pointer));
            //    //    assert(S != mResourceInstanceRelease.end());

            //    //    S->second->mRefCount++;
            //    //}
            //    //else
            //    //{
            //    //    if (Dest.partialRef.IsPointer()) ReleaseRef(Dest);
            //    //}

            //    //Dest.partialRef = URef.partialRef;
                //Dest.typeGUID = URef.typeGUID;
            //}

            uint64_t getResourceCount()
            {
                return mAssetRefs.size();
                //assert(mResourceInstance.size() == mResourceInstanceRelease.size());
                //return static_cast<int>(mResourceInstance.size());
            }

        protected:

            Details::InstanceInfo& AllocRscInfo(void)
            {
                //auto pTemp = mpInfoBufferEmptyHead;
                //assert(pTemp);

                //Details::InstanceInfo* pNext = reinterpret_cast<Details::InstanceInfo*>(mpInfoBufferEmptyHead->mpData);
                //mpInfoBufferEmptyHead = pNext;

                //return *pTemp;
            }

            void ReleaseRscInfo(Details::InstanceInfo& RscInfo)
            {
                // Add this resource info to the empty chain
                //RscInfo.mpData = mpInfoBufferEmptyHead;
                //mpInfoBufferEmptyHead = &RscInfo;
            }

            void FullInstanceInfoAlloc(void* pRsc, GUID RscGUID, TypeGUID typeguid)
            {
                //auto& RscInfo = AllocRscInfo();

                //RscInfo.mpData = pRsc;
                //RscInfo.guid = RscGUID;
                //RscInfo.typeGUID = typeguid;
                //RscInfo.mRefCount = 1;

                //std::uint64_t HashID = RscGUID ^ typeguid;
                //mResourceInstance.emplace(HashID, &RscInfo);
                //mResourceInstanceRelease.emplace(reinterpret_cast<std::uint64_t>(pRsc), &RscInfo);
            }

            void FullInstanceInfoRelease(Details::InstanceInfo& RscInfo)
            {
                //// Release references in the hashs maps
                //mResourceInstance.erase(RscInfo.guid ^ RscInfo.typeGUID);
                //mResourceInstanceRelease.erase(reinterpret_cast<std::uint64_t>(RscInfo.mpData));

                //// Add this resource info to the empty chain
                //ReleaseRscInfo(RscInfo);
            }

            constexpr static auto max_resources_v = 1024;

            std::unordered_map<TypeKey, Details::UniversalInfo>  mRegisteredTypes;

            //keep in mind that any instance of Ref<T> always has a minimum of 1 reference
            std::unordered_map<TypeAssetKey, RefAny> mAssetRefs; //bitwise xor the typeguid and guid for the key;
            //std::unordered_map<std::uint64_t, Details::InstanceInfo*>  mResourceInstance;
            //std::unordered_map<std::uint64_t, Details::InstanceInfo*>  mResourceInstanceRelease;
            //Details::InstanceInfo* mpInfoBufferEmptyHead{ nullptr };
            //std::array<Details::InstanceInfo, max_resources_v>         mInfoBuffer;
        };
	}
}