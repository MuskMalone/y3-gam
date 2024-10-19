#pragma once
#include <Singleton/ThreadSafeSingleton.h>
#include "Core/GUID.h"
#include "SmartPointer.h"
#include <rttr/type.h>
#include <any>
#include <filesystem>
#include <type_traits>
#include "AssetUtils.h"
#include <Events/EventCallback.h>

namespace IGE {
	namespace Assets {
      class AssetManager : public ThreadSafeSingleton<AssetManager>
      {
      private: 
          EVENT_CALLBACK_DECL(HandleAddFiles);

          template <typename, typename, typename = void>
          struct HasStaticImport : std::false_type {};
          template <typename, typename, typename = void>
          struct HasStaticLoad : std::false_type {};
          template <typename, typename, typename = void>
          struct HasStaticUnload : std::false_type {};
          // Generalized trait to check for a static function with a specific return type
          template <typename T, typename ReturnType, typename... Args>
          struct HasStaticImport<T, ReturnType(Args...), std::void_t<decltype(T::Import(std::declval<Args>()...))>>
              : std::is_same<decltype(T::Import(std::declval<Args>()...)), ReturnType> {};

          template <typename T, typename ReturnType, typename... Args>
          struct HasStaticLoad<T, ReturnType(Args...), std::void_t<decltype(T::Load(std::declval<Args>()...))>>
              : std::is_same<decltype(T::Load(std::declval<Args>()...)), ReturnType> {};

          template <typename T, typename ReturnType, typename... Args>
          struct HasStaticUnload<T, ReturnType(Args...), std::void_t<decltype(T::Unload(std::declval<Args>()...))>>
              : std::is_same<decltype(T::Unload(std::declval<Args>()...)), ReturnType> {};
      private:
          using RefAny = std::any;
          using TypeKey = std::uint64_t;
          using TypeAssetKey = std::uint64_t;
      private: 
          template <typename T>
          auto GetTypeName() { return rttr::type::get<T>().get_name().to_string(); }
      public:
          //-------------------------------------------------------------------------
          AssetManager();

          //-------------------------------------------------------------------------
          template< typename...T_ARGS >
          void RegisterTypes()
          {
              //
              // Insert all the types into the hash table
              //

              ([&]< typename T >(T*)
              {
                  // Static assertions to check the existence of functions
                  static_assert(HasStaticImport<T, GUID(std::string const&)>::value, "Ref<T> must have Import returning IGE::Assets::GUID.");
                  static_assert(HasStaticLoad<T, void* (GUID)>::value, "Ref<T> must have Load returning void*.");
                  static_assert(HasStaticUnload<T, void(T*, GUID)>::value, "Ref<T> must have Unload returning void.");                    auto name = GetTypeName<T>();
                  auto typeguid = TypeGUID{ name };
                  mRegisteredTypes.emplace
                  (
                      typeguid,
                      Details::UniversalInfo{ typeguid,
                      [](GUID GUID) { return T::Load(GUID); },
                      [](void* pData, GUID GUID) {
                          T::Unload(reinterpret_cast<T*>(pData), GUID);
                      },
                      name }
                  );
              }(reinterpret_cast<T_ARGS*>(0))
                  , ...
                  );
          }
          template <typename T>
          //returns the guid seeded by the absolute path to the folder in assets
          GUID ImportAsset(std::string const& filepathstr){//std::filesystem::path const& filepath) { //
              //get the absolute file path so that there cant be any duplicates
              std::string absolutepath{filepathstr};
              GUID guid{ absolutepath };
              TypeGUID typeguid{ GetTypeName<T>() };
              if (IsValidFilePath(absolutepath)) {
                  std::filesystem::path filepath{absolutepath};
                  guid = GUID{ std::filesystem::absolute(filepath).string() };
                  absolutepath = std::filesystem::absolute(filepath).string();
              }
              else {
                  guid = GUID{ filepathstr  };
                  absolutepath = filepathstr;
              }
              TypeAssetKey key{ typeguid ^ guid };
              if (mAssetRefs.find(key) == mAssetRefs.end()) { //if asset not found, generate a new guid
                  guid = T::Import(absolutepath);
                  key = TypeAssetKey{ typeguid ^ guid };

                  Ref<T> ref{
                      IGE::Assets::UniversalRef{ PartialRef{ guid, nullptr }, typeguid }, 
                      IGE::Assets::Details::UniversalInfo{ mRegisteredTypes.at(typeguid) },
                      IGE::Assets::Details::InstanceInfo{ guid, typeguid, 1, absolutepath, false }
                  };

                  mAssetRefs.emplace(key, ref);
                  return guid;
              }
              else {
                  TypeAssetKey key{ typeguid ^ guid };

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
          //template< typename T > not needed
          //T* GetAsset(Ref<T>& ref)
          //{
          //    //// If we already have the resource return now
          //    if (ref.partialRef.IsPointer()) return reinterpret_cast<T*>(ref.partialRef.pointer);
          //    else return nullptr; //should i throw here?

          //}

          //-------------------------------------------------------------------------
          template<typename T>
          Ref<T> GetAsset(GUID const& guid)
          {
              TypeGUID typeguid{ GetTypeName<T>() };
              TypeAssetKey key{ typeguid ^ guid };
              if (mAssetRefs.find(key) != mAssetRefs.end()) {
                  auto out{ std::any_cast<Ref<T>>(mAssetRefs.at(key)) };
                  return out;
              }
              else throw std::exception(); // should be a specialized exception
          }

          //-------------------------------------------------------------------------
          template <typename T>
          void LoadRef(Ref<T>& ref) {
              ref.Load();
          }
          template <typename T>
          void LoadRef(GUID const& guid) {
              TypeGUID typeguid{ GetTypeName<T>() };
              TypeAssetKey key{ typeguid ^ guid };
              if (mAssetRefs.find(key) != mAssetRefs.end())
                  LoadRef<T>(std::any_cast<Ref<T>&>(mAssetRefs.at(key)));
          }
          template <typename T>
          GUID LoadRef(std::string const& fp) {
              std::string filepath {fp};
              if (IsValidFilePath(fp) && !IsPathWithinDirectory(fp, gAssetsDirectory)) throw std::runtime_error("file is not within assets dir");
              GUID guid{ IsValidFilePath(fp) ? GetAbsolutePath(fp) : fp }; //to account for keys that are not file paths etc "Cube"
              TypeGUID typeguid{ GetTypeName<T>() };
              TypeAssetKey key{ typeguid ^ guid };
              if (mAssetRefs.find(key) != mAssetRefs.end()) {
                  LoadRef<T>(std::any_cast<Ref<T>&>(mAssetRefs.at(key)));
                  return guid;
              }
              else {
                  ImportAsset<T>(fp);
                  LoadRef<T>(std::any_cast<Ref<T>&>(mAssetRefs.at(key)));
                  return guid;
              }
          }
          template< typename T >
          void UnloadRef(Ref<T>& ref){
              ref.Unload();
          }

          template< typename T >
          void UnloadRef(GUID const& guid) {
              TypeGUID typeguid{ GetTypeName<T>() };
              TypeAssetKey key{ typeguid ^ guid };
              if (mAssetRefs.find(key) != mAssetRefs.end())
                  return UnloadRef<T>(std::any_cast<Ref<T>&>(mAssetRefs.at(key)));
          }

          template< typename T >
          Details::InstanceInfo GetInstanceInfo(Ref<T>& ref) const
          {
              return ref.GetInfo();
          }
          template <typename T>
          bool IsGUIDValid(GUID const& guid) {
              TypeGUID typeguid{ GetTypeName<T>() };
              TypeAssetKey key{ typeguid ^ guid };
              return (mAssetRefs.find(key) != mAssetRefs.end());
          }

      protected:

          //Details::InstanceInfo& AllocRscInfo(void)
          //{
          //}
          //constexpr static auto max_resources_v = 1024;

          std::unordered_map<TypeKey, Details::UniversalInfo>  mRegisteredTypes;

          //keep in mind that any instance of Ref<T> always has a minimum of 1 reference
          std::unordered_map<TypeAssetKey, RefAny> mAssetRefs; //bitwise xor the typeguid and guid for the key;
      };
	}
}