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
#include "Asset/AssetMetadata.h"
#include <mutex>

#ifndef DISTRIBUTION
//#define CHECK_UNUSED_ASSETS
#endif

#define IGE_ASSETMGR IGE::Assets::AssetManager::GetInstance()
#define IGE_REF(type, guid) IGE::Assets::AssetManager::GetInstance().GetAsset<type>(guid)
namespace IGE {
	namespace Assets {
        const std::string cAssetProjectSettingsPath{ gAssetsDirectory }; // just the same directory as the exe
        const std::string cSettingsFileName{ "metadata.igeproj" };
        const std::string cAssetMetadataFileExtension{ ".igemeta" };
        class AssetManager : public ThreadSafeSingleton<AssetManager>
      {
      private: 
          EVENT_CALLBACK_DECL(HandleAddFiles);
          EVENT_CALLBACK_DECL(OnRemapGUID);

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
          using ImportFunc = std::function<GUID(std::string const&, AssetMetadata::AssetProps)>;
          using DeleteFunc = std::function<void(GUID const&)>;
      private: 
          template <typename T>
          std::string GetTypeName() { return rttr::type::get<T>().get_name().to_string(); }
          void Initialize();
          std::string CreateProjectFile() const;
      public:
          //-------------------------------------------------------------------------
          AssetManager();
          ~AssetManager();
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
                  static_assert(HasStaticImport<T, GUID(std::string const&, std::string&, AssetMetadata::AssetProps&)>::value, "Ref<T> must have Import returning IGE::Assets::GUID.");
                  static_assert(HasStaticLoad<T, void* (GUID)>::value, "Ref<T> must have Load returning void*.");
                  static_assert(HasStaticUnload<T, void(T*, GUID)>::value, "Ref<T> must have Unload returning void.");
                  auto name = GetTypeName<T>();
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
                  mRegisteredTypeNames.emplace(
                      name
                  );
                  // using lambda to get around default arg (can't specify that in an std::function decl)
                  mRegisterTypeImports.emplace(
                    name,
                    [this](const std::string& filepath, AssetMetadata::AssetProps metadata = {}) {
                      return this->ImportAsset<T>(filepath, metadata);
                    }
                  );

                  mRegisterTypeDeletes.emplace(
                      name,
                      std::bind(&AssetManager::DeleteAsset<T>, this, std::placeholders::_1)
                  );
              }(reinterpret_cast<T_ARGS*>(0))
                  , ...
                  );
          }
          //returns the guid seeded by the absolute path to the folder in assets
          // CE: added 2nd arg to allow for importing with metadata
          template <typename T>
          GUID ImportAsset(std::string const& filepathstr, AssetMetadata::AssetProps metadata = {}) {
              std::lock_guard<std::mutex> lock(mAssetsMutex);

              //get the absolute file path so that there cant be any duplicates
              //std::string absolutepath{filepathstr};
              GUID guid{};
              TypeGUID typeguid{ GetTypeName<T>() };
              try {
                  guid = PathToGUID(filepathstr); // in case of duplicates
                  return guid;
              }
              catch (...) {
                  Debug::DebugLogger::GetInstance().LogInfo("new asset " + filepathstr + " began importing!");
                  // do nothing
              }

              //if (IsValidFilePath(absolutepath)) {
              //    std::filesystem::path filepath{absolutepath};
              //    guid = GUID{ std::filesystem::absolute(filepath).string() };
              //    absolutepath = std::filesystem::absolute(filepath).string();
              //}
              //else {
              //    guid = GUID{ filepathstr  };
              //    absolutepath = filepathstr;
              //}
              TypeAssetKey key{ typeguid ^ guid };
              if (mAssetRefs.find(key) == mAssetRefs.end()) { //if asset not found, generate a new guid
                  // newFp represents the file that is non compiled, 
                  // not the compiled file that will be used during load time
                  std::string newFp{};
                  metadata.modified = true;
                  // this is to get the new FP
                  guid = T::Import(filepathstr, newFp, metadata); 
                  key = TypeAssetKey{ typeguid ^ guid };
                  if (!guid.IsValid())
                      throw Debug::Exception<AssetManager>(Debug::LVL_ERROR, Msg("guid invalid after calling asset import"));

                  // register under the respective categories in metadata, 
                  AssetMetadata::IGEProjProperties& allmetadata{ mMetadata.mAssetProperties };
                  std::string const assetCategory{ GetTypeName<T>() };
                  mMetadata.Emplace(assetCategory, guid, metadata);
                  mPath2GUIDRegistry.emplace(newFp, guid);
                  mGUID2PathRegistry.emplace(guid, newFp);

                  SaveMetadata(metadata, assetCategory, guid);

                  return guid;
              }
              else {
                  std::string newFp{};
                  AssetMetadata::AssetProps metadata{};
                  metadata.modified = true;
                  //runs the import function again even if theres a guid
                  T::Import(filepathstr, newFp, metadata);
                  Ref<T> ref { std::any_cast<Ref<T>>(mAssetRefs.at(key)) };
                  return ref.mInstance.partialRef.guid;
              }

          }
          template <typename T>
          void DeleteAsset(GUID const& guid) {
              std::string const type{ GetTypeName<T>() };
              TypeGUID typeguid{ type };
              TypeAssetKey key{ typeguid ^ guid };
              //removes the file from all maps

              std::lock_guard<std::mutex> lock(mAssetsMutex);

              auto path{ GUIDToPath(guid) };
              AssetMetadata::AssetCategory& category{ mMetadata.mAssetProperties.at(type) };
              if (mAssetRefs.find(key) != mAssetRefs.end()) {
                  Ref<T> ref { std::any_cast<Ref<T>>(mAssetRefs.at(key)) };
                  //std::filesystem::remove(ref.mInfo.filepath);
                  mAssetRefs.erase(key);
                  //ref auto unloads if its the last reference
              }
              if (category.find(guid) != category.end()) {
                  category.erase(guid);
              }
              if (mPath2GUIDRegistry.find(path) != mPath2GUIDRegistry.end()) {
                  mPath2GUIDRegistry.erase(path);
              }
              if (mGUID2PathRegistry.find(guid) != mGUID2PathRegistry.end()) {
                  mGUID2PathRegistry.erase(guid);
              }

              std::string const fileName{ GetFileName(path) };
              auto metaToRemove{ cAssetProjectSettingsPath + type + "\\" + fileName + "." + std::to_string(static_cast<uint64_t>(guid)) + cAssetMetadataFileExtension };
              std::filesystem::remove(metaToRemove);

              // remove compiled version
              std::string const compiledPath{ gAssetsDirectory + type + "\\" + "Compiled\\" };
              if (std::filesystem::exists(compiledPath)) {
                // not sure if there's a btr way then iterating through the whole directory
                for (auto const& file : std::filesystem::directory_iterator(std::filesystem::directory_iterator(compiledPath))) {
                  if (file.path().stem() == fileName) {
                    std::filesystem::remove(file);
                    break;
                  }
                }
              }
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

              std::lock_guard<std::mutex> lock(mAssetsMutex);

#ifdef CHECK_UNUSED_ASSETS
              {
                std::string fp{ GUIDToPath(guid) };
                assetsSoFar.emplace(fp);
                assetsPerScene.back().second.emplace(std::move(fp));
              }
#endif

              if (mAssetRefs.find(key) != mAssetRefs.end()) {
                  return std::any_cast<Ref<T>>(mAssetRefs.at(key));
              }
              else
                  throw Debug::Exception<AssetManager>(Debug::LVL_ERROR, Msg("no such asset could be found with guid " + std::to_string(static_cast<uint64_t>(guid)))); // should be a specialized exception
          }
          private:
          template <typename T> 
          void InstantiateRefInAssetRefs(GUID const& guid, TypeGUID const& typeguid, std::string const& fp) {
              TypeAssetKey key{ typeguid ^ guid };
              Ref<T> ref{
                  IGE::Assets::UniversalRef{ PartialRef{ guid, nullptr }, typeguid },
                      IGE::Assets::Details::UniversalInfo{ mRegisteredTypes.at(typeguid) },
                      IGE::Assets::Details::InstanceInfo{ guid, typeguid, 1, fp, false }
              };

              mAssetRefs.emplace(key, ref);
          }
          public:
          //-------------------------------------------------------------------------
          template <typename T>
          void LoadRef(Ref<T>& ref) {
              ref.Load();
          }
          template <typename T>
          GUID LoadRef(GUID const& guid) {
              TypeGUID typeguid{ GetTypeName<T>() };
              TypeAssetKey key{ typeguid ^ guid };

              std::lock_guard<std::mutex> lock(mAssetsMutex);

              std::string const fp{ GUIDToPath(guid) };
#ifdef CHECK_UNUSED_ASSETS
              assetsSoFar.emplace(fp);
              assetsPerScene.back().second.emplace(fp);
#endif

              if (mAssetRefs.find(key) != mAssetRefs.end()) {
                  LoadRef<T>(std::any_cast<Ref<T>&>(mAssetRefs.at(key)));
                  return guid;
              }
              else {
                  //I AM INSTANTIATING THE REF HERE INSTEAD OF IMPORT
                  if (mPath2GUIDRegistry.find(fp) != mPath2GUIDRegistry.end()) {
                      InstantiateRefInAssetRefs<T>(guid, typeguid, fp);
                      LoadRef<T>(std::any_cast<Ref<T>&>(mAssetRefs.at(key)));
                      return guid;
                  }
                  else
                      throw Debug::Exception<AssetManager>(Debug::LVL_ERROR, Msg("no such filepath imported"));
              }
          }

          // if you wanna do smth like LoadRef<Class>("SomeSpecialString"), do it with this function. 
          template <typename T>
          GUID LoadRef(std::string const& fp) {
#ifdef CHECK_UNUSED_ASSETS
            assetsSoFar.emplace(fp);
            assetsPerScene.back().second.emplace(fp);
#endif

              std::string filepath {fp};
              if (IsValidFilePath(fp) && !IsPathWithinDirectory(fp, gAssetsDirectory))
                  throw Debug::Exception<AssetManager>(Debug::EXCEPTION_LEVEL::LVL_CRITICAL, Msg("file is not within assets dir"));

              //GUID guid{ IsValidFilePath(fp) ? fp : fp }; //to account for keys that are not file paths etc "Cube"
              GUID guid{ };

              std::lock_guard<std::mutex> lock(mAssetsMutex);

              try {
                  guid = GUID{ PathToGUID(fp) };
              }
              catch ([[maybe_unused]] Debug::Exception<AssetManager> const& e) { // if there isnt any valid file path, just create a new guid
                  guid = GUID{ GUID::Seed{} };
              }
              TypeGUID typeguid{ GetTypeName<T>() };
              TypeAssetKey key{ typeguid ^ guid };
              if (mAssetRefs.find(key) != mAssetRefs.end()) {
                  LoadRef<T>(std::any_cast<Ref<T>&>(mAssetRefs.at(key)));
                  return guid;
              }
              else {

                  //ImportAsset<T>(fp);
                  //I AM INSTANTIATING THE REF HERE INSTEAD OF IMPORT
                  //if (mPath2GUIDRegistry.find(fp) != mPath2GUIDRegistry.end()) {
                  if (!IsValidFilePath(fp)) { // if it is a special string like "cube"
                      mGUID2PathRegistry.emplace(guid, fp);
                      mPath2GUIDRegistry.emplace(fp, guid);
                  }
                      InstantiateRefInAssetRefs<T>(guid, typeguid, fp);
                      LoadRef<T>(std::any_cast<Ref<T>&>(mAssetRefs.at(key)));
                      return guid;
                  //}
                  //else
                  //    throw Debug::Exception<AssetManager>(Debug::LVL_ERROR, Msg("no such filepath imported"));
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

              std::lock_guard<std::mutex> lock(mAssetsMutex);

              if (mAssetRefs.find(key) != mAssetRefs.end())
                  return UnloadRef<T>(std::any_cast<Ref<T>&>(mAssetRefs.at(key)));
          }


          //i do not perform any checks here, reload at your own risk
          template<typename T>
          void ReloadRef(Ref<T>& ref) {
              if (ref.GetInfo().refCount > 0) {
                  ref.Unload();
                  ref.Load();
              }
          }
          template<typename T>
          void ReloadRef(GUID const& guid) {
              TypeGUID typeguid{ GetTypeName<T>() };
              TypeAssetKey key{ typeguid ^ guid };

              std::lock_guard<std::mutex> lock(mAssetsMutex);

              if (mAssetRefs.find(key) != mAssetRefs.end())
                  return ReloadRef<T>(std::any_cast<Ref<T>&>(mAssetRefs.at(key)));
          }
          template<typename T>
          void ReloadRef(std::string const& fp) {
              ReloadRef<T>(PathToGUID(fp));
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

              std::lock_guard<std::mutex> lock(mAssetsMutex);

              return (mAssetRefs.find(key) != mAssetRefs.end());
          }

          template <typename T>
          AssetMetadata::AssetProps& GetMetadata(GUID const& guid) {
            std::lock_guard<std::mutex> lock(mAssetsMutex);
            return GetMetadataInternal<T>(guid);
          }
          template <typename T>
          AssetMetadata::AssetProps const& GetCMetadata(GUID const& guid) const {
              return GetMetadata<T>(guid);
          }

          template <typename T>
          void ChangeAssetPath(GUID const& guid, std::string newPath) {
              try {
                  std::lock_guard<std::mutex> lock(mAssetsMutex);

                  //update the metadata stuff
                  auto& metadata{ GetMetadataInternal<T>(guid) };
                  metadata.modified = true;
                  auto const& oldPath{ metadata.metadata.at("path") };
                  //since guid is already in the metadata, safe to assume that it will be in the registries as well
                  mGUID2PathRegistry.at(guid) = newPath;
                  mPath2GUIDRegistry.emplace(newPath, guid);
                  mPath2GUIDRegistry.erase(oldPath);

                  auto metaToRemove{ cAssetProjectSettingsPath + GetTypeName<T>() + "\\" + GetFileName(oldPath) + "." + std::to_string(static_cast<uint64_t>(guid)) + cAssetMetadataFileExtension };
                  std::remove(metaToRemove.c_str());

                  metadata.metadata.at("path") = newPath;
                  //remove the old metadata file

                  SaveMetadata(metadata, GetTypeName<T>(), guid);
              }
              catch ([[maybe_unused]] Debug::Exception<AssetManager> const& e) {
                  Debug::DebugLogger::GetInstance().LogWarning("guid " + std::to_string(guid) + " does not exist within metadata");
              }
              catch (...) {
                  Debug::DebugLogger::GetInstance().LogWarning("guid " + std::to_string(guid) + " with filepath " + newPath + " could not be changed");
              }
          }

        private:
          // used internally by asset manager (no lock_guard to prevent deadlocks)
          template <typename T>
          AssetMetadata::AssetProps& GetMetadataInternal(GUID const& guid) {
            auto category{ GetTypeName<T>() };
            auto& props{ mMetadata.mAssetProperties };
            if (props.find(category) != props.end()) {
              auto& cat{ props.at(category) };
              if (cat.find(guid) != cat.end()) {
                return cat.at(guid);
              }
              else throw Debug::Exception<AssetManager>(Debug::LVL_ERROR, Msg("asset category has no such guid: " + std::to_string(guid)));
            }
            else throw Debug::Exception<AssetManager>(Debug::LVL_ERROR, Msg("asset metadatas have no such category: " + category));
          }

          AssetMetadata mMetadata;
          std::unordered_map<std::string, GUID> mPath2GUIDRegistry;
          std::unordered_map<GUID, std::string> mGUID2PathRegistry;

          std::unordered_map<TypeKey, Details::UniversalInfo>  mRegisteredTypes;
          std::unordered_set<std::string> mRegisteredTypeNames;

#ifdef CHECK_UNUSED_ASSETS
          EVENT_CALLBACK_DECL(OnSceneLoad);
          std::vector<std::pair<std::string, std::set<std::string>>> assetsPerScene{};
          std::set<std::string> assetsSoFar{};
#endif

          //template function instantiation

          std::unordered_map<std::string, ImportFunc> mRegisterTypeImports;
          std::unordered_map<std::string, DeleteFunc> mRegisterTypeDeletes;

          //keep in mind that any instance of Ref<T> always has a minimum of 1 reference
          std::unordered_map<TypeAssetKey, RefAny> mAssetRefs; //bitwise xor the typeguid and guid for the key;
          std::mutex mAssetsMutex;

        public: //get set
            std::string const& GUIDToPath(GUID const& guid);

            GUID PathToGUID(std::string const& path);
            ImportFunc ImportFunction(std::string const& type);
            DeleteFunc DeleteFunction(std::string const& type);
            void SaveMetadata(AssetMetadata::AssetProps& metadata, std::string const& category, GUID guid) const;
            void SaveAllMetadata() const;

            template <typename T>
            bool IsTypeRegistered() {
                return mRegisteredTypeNames.find(GetTypeName<T>()) != mRegisteredTypeNames.end();
            }
            bool IsTypeRegistered(std::string const&);
        };
	}
}