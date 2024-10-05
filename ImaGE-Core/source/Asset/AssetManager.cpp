#include <pch.h>
#include "AssetManager.h"
#include "Events/EventManager.h"
#include <Asset/Assetables/AssetExtensions.h>

#include <Asset/Assetables/Mesh/MeshAsset.h>
#include <Asset/Assetables/Audio/AudioAsset.h>
#include <Asset/Assetables/Texture/TextureAsset.h>
namespace IGE {
	namespace Assets {
		std::shared_ptr<AssetManager> AssetManager::_mSelf;
		std::mutex AssetManager::_mMutex;
		std::shared_ptr<AssetManager> AssetManager::GetInstance()
		{
			std::lock_guard<std::mutex> lock(_mMutex);
			if (_mSelf == nullptr) {

				_mSelf = std::make_shared<AssetManager>();
				SUBSCRIBE_CLASS_FUNC(Events::EventType::ADD_FILES, &AssetManager::HandleAddFiles, _mSelf);
			}
			return _mSelf;
		}
		EVENT_CALLBACK_DEF(AssetManager, HandleAddFiles) {
			auto const& paths{ CAST_TO_EVENT(Events::AddFilesFromExplorerEvent)->mPaths };
			for (std::string const& file : paths) {
				//@TODO: use reflection to invoke without hardcoding
				auto ext{ GetFileExtension(file) };
				if (cMeshExtensions.find(ext) != cMeshExtensions.end()){
					ImportAsset<MeshAsset>(file);
				}
				else if (cImageExtensions.find(ext) != cImageExtensions.end()) {
					ImportAsset<TextureAsset>(file);
				}
				else if (cAudioExtensions.find(ext) != cAudioExtensions.end()) {
					ImportAsset<AudioAsset>(file);
				}
			}

		}

	}
}
