#include <pch.h>
#include "AssetManager.h"
#include "Events/EventManager.h"
#include <Asset/Assetables/AssetExtensions.h>
#include <Asset/AssetUtils.h>
#include <Asset/Assetables/Assetables.h>

#include "Serialization/Serializer.h"
#include "Serialization/Deserializer.h"

#include <DebugTools/DebugLogger/DebugLogger.h>
#include <DebugTools/Exception/Exception.h>
namespace IGE {
	namespace Assets {
		void AssetManager::Initialize()
		{
			HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
			if (FAILED(hr)) {
				throw std::runtime_error{"Error: CoInitializeEx failed!"};
			}
			//#endif

			// +++++++++++++++++++++++ ASSET REGISTRY ++++++++++++++++++++++++++++++++++++++++
			RegisterTypes<
				IGE::Assets::TextureAsset,
				IGE::Assets::AudioAsset,
				IGE::Assets::ModelAsset,
				IGE::Assets::PrefabAsset,
				IGE::Assets::FontAsset,
				IGE::Assets::ShaderAsset,
				IGE::Assets::MaterialAsset
			>();
			// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			auto fp{ CreateProjectFile() }; // create project file if it doesnt exist
			Serialization::Deserializer::DeserializeAny(mMetadata, fp);
			//load the guid/path registries
			for (std::pair<std::string, AssetMetadata::AssetCategory> const& category : mMetadata.mAssetProperties) {
				for (std::pair<AssetMetadata::AssetPropsKey, AssetMetadata::AssetProps> const& entry : category.second) {
					//if the attribute path doesnt exist :(
					if (entry.second.find("path") == entry.second.end())
						throw Debug::Exception<AssetManager>(Debug::EXCEPTION_LEVEL::LVL_CRITICAL, Msg("asset " + std::to_string(entry.first) + " doesnt have a path!"));
					mGUID2PathRegistry.emplace(GUID{ entry.first }, entry.second.at("path"));
					mPath2GUIDRegistry.emplace(entry.second.at("path"), GUID(entry.first));
				}
			}
		}
		std::string AssetManager::CreateProjectFile() const
		{
			std::string const out{ cAssetProjectSettingsPath + cSettingsFileName };

			// Add these conditionals so that the installer will work (unable to write to log
			// file as app does not have permission to do so
			// @TODO: Write backup files to a writable location such as {userdata}
			
			// No files should be copied/created in the distribution build
#ifndef DISTRIBUTION
			// make a copy of the file as backup before overwriting it
			// create backup directory if it doesn't already exist
			if (!std::filesystem::exists(gBackupDirectory))
			{
				if (std::filesystem::create_directory(gBackupDirectory)) {
					Debug::DebugLogger::GetInstance().LogInfo(std::string("Backup directory doesn't exist. Created at: ") + gBackupDirectory);
				}
				else {
					Debug::DebugLogger::GetInstance().LogWarning("Unable to create temp directory at: " + std::string(gBackupDirectory) + ". Scene reloading features may be unavailable!");
				}
			}
#endif
			// now copy the file over
			if (IsValidFilePath(out)) {
				// No files should be copied/created in the distribution build
#ifndef DISTRIBUTION
				std::filesystem::copy(out, gBackupDirectory, std::filesystem::copy_options::overwrite_existing);
#endif
				return out;
			}

			/* This was causing issues
			std::ofstream file(out);
			file.close();
			return out;
			*/
		}
		AssetManager::AssetManager() {
			SUBSCRIBE_CLASS_FUNC(Events::EventType::REGISTER_FILES, &AssetManager::HandleAddFiles, this);
			Initialize();
			 //code snippet to "manufacture" all the data needed for importing
			 //assumes that all the files are imported as is
			//for (const auto& folder : mRegisteredTypeNames) {
			//	namespace fs = std::filesystem;
			//	auto assetpath{ std::string(gAssetsDirectory) + "\\" + folder};
			//	fs::path subDir{ assetpath };
			//	CreateDirectoryIfNotExists(assetpath);
			//	for (const auto& entry : fs::directory_iterator(subDir)) {
			//		std::string p{ entry.path().string()};
			//		mRegisterTypeImports[folder](p);
			//	}
			//}
			////to delete
			//auto fp{ CreateProjectFile() };
			//Serialization::Serializer::SerializeAny(mMetadata, fp);

			// erase all files that no longer exist from metadata
			bool removed{ false };
			for (auto& entry : mMetadata.mAssetProperties) {
				for (auto iter{ entry.second.begin() }; iter != entry.second.end();) {
					if (!iter->second.contains("path")) {
#ifdef _DEBUG
						std::cout << "[AssetManager] Removed invalid metadata for asset " << iter->first << "\n";
#endif
					}

					if (!std::filesystem::exists(iter->second["path"])) {
#ifdef _DEBUG
						std::cout << "[AssetManager] Removed " << iter->second["path"] << " from asset metadata as it no longer exists\n";
#endif
						removed = true;
						iter = entry.second.erase(iter);
					}
					else {
						++iter;
					}
				}
			}

			if (removed) {
				SaveMetadata();
			}
		}

		AssetManager::~AssetManager()
		{
			SaveMetadata();
		}

		void AssetManager::SaveMetadata() const {
			auto fp{ CreateProjectFile() }; // get the project file if exists
			Serialization::Serializer::SerializeAny(mMetadata, fp);
		}

		std::string AssetManager::GUIDToPath(GUID const& guid) {
			if (mGUID2PathRegistry.find(guid) == mGUID2PathRegistry.end())
				throw Debug::Exception<AssetManager>(Debug::EXCEPTION_LEVEL::LVL_CRITICAL, Msg("GUID " + std::to_string(static_cast<uint64_t>(guid)) + " doesnt have path"));
			return mGUID2PathRegistry.at(guid);
		}

		GUID AssetManager::PathToGUID(std::string const& path) {
			if (mPath2GUIDRegistry.find(path) == mPath2GUIDRegistry.end())
				throw Debug::Exception<AssetManager>(Debug::EXCEPTION_LEVEL::LVL_CRITICAL, Msg("path " + path + " doesnt have GUID"));
			return mPath2GUIDRegistry.at(path);
		}

		AssetManager::ImportFunc AssetManager::ImportFunction(std::string const& type)
		{
			// TODO
			return mRegisterTypeImports[type];
		}

		AssetManager::DeleteFunc AssetManager::DeleteFunction(std::string const& type)
		{
			return mRegisterTypeDeletes[type];
		}
		
		EVENT_CALLBACK_DEF(AssetManager, HandleAddFiles) {
			auto const& paths{ CAST_TO_EVENT(Events::RegisterAssetsEvent)->mPaths };
			for (std::string const& file : paths) {
				//auto file{ std::filesystem::path{f}.relative_path().string() };
				//@TODO: use reflection to invoke without hardcoding
				auto ext{ GetFileExtension(file) };
				std::string folder{};
				
				//finds the folder
				//then breaks
				for (auto const& filetype : cDirectoryToExtensions) {

					if (filetype.second.find(ext) != filetype.second.end()) {
						folder = filetype.first;
						break;
					}
				}
				if (mRegisterTypeImports.find(folder) != mRegisterTypeImports.end()) {
					mRegisterTypeImports[folder](file);
					Debug::DebugLogger::GetInstance().LogInfo("Added " + file + " to assets");
				}else{
					Debug::DebugLogger::GetInstance().LogWarning(file + " no asset category found");
				}
			}

		}

	}
}
