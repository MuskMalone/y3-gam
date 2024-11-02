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
				IGE::Assets::FontAsset
			>();
			// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			auto fp{ CreateProjectFile() }; // create project file if it doesnt exist
			Serialization::Deserializer::DeserializeAny(mMetadata, fp);
			//load the guid/path registries
			for (std::pair<std::string, AssetMetadata::AssetCategory> const& category : mMetadata.mAssetProperties) {
				for (std::pair<AssetMetadata::AssetPropsKey, AssetMetadata::AssetProps> const& entry : category.second) {
					//if the attribute path doesnt exist :(
					if (entry.second.find("path") == entry.second.end())
						throw Debug::Exception<AssetManager>(Debug::EXCEPTION_LEVEL::LVL_CRITICAL, Msg("asset doesnt have a path!"));
					mGUID2PathRegistry.emplace(GUID{ entry.first }, entry.second.at("path"));
					mPath2GUIDRegistry.emplace(entry.second.at("path"), GUID(entry.first));
				}
			}
		}
		std::string AssetManager::CreateProjectFile()
		{
			auto out{ cAssetProjectSettingsPath + cSettingsFileName };
			if (IsValidFilePath(out)) return out;
			std::ofstream file(out);
			file.close();
			return out;
		}
		AssetManager::AssetManager() {
			SUBSCRIBE_CLASS_FUNC(Events::EventType::ADD_FILES, &AssetManager::HandleAddFiles, this);
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
		}

		AssetManager::~AssetManager()
		{
			auto fp{ CreateProjectFile() }; // get the project file if exists
			Serialization::Serializer::SerializeAny(mMetadata, fp);
		}

		std::string AssetManager::GUIDToPath(GUID const& guid) {
			if (mGUID2PathRegistry.find(guid) == mGUID2PathRegistry.end())
				throw Debug::Exception<AssetManager>(Debug::EXCEPTION_LEVEL::LVL_CRITICAL, Msg("GUID doesnt have path"));
			return mGUID2PathRegistry.at(guid);
		}

		GUID AssetManager::PathToGUID(std::string const& path) {
			if (mPath2GUIDRegistry.find(path) == mPath2GUIDRegistry.end())
				throw Debug::Exception<AssetManager>(Debug::EXCEPTION_LEVEL::LVL_CRITICAL, Msg("path doesnt have GUID"));
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
			auto const& paths{ CAST_TO_EVENT(Events::AddFilesFromExplorerEvent)->mPaths };
			for (std::string const& file : paths) {
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

				mRegisterTypeImports[folder](file);
				Debug::DebugLogger::GetInstance().LogInfo("Added " + file + " to assets");
			}

		}

	}
}
