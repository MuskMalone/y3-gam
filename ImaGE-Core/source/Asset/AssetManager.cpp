#include <pch.h>
#include "AssetManager.h"

namespace IGE {
	namespace Assets {
		std::shared_ptr<AssetManager> AssetManager::_mSelf;
		std::mutex AssetManager::_mMutex;
		std::shared_ptr<AssetManager> AssetManager::GetInstance()
		{
			std::lock_guard<std::mutex> lock(_mMutex);
			if (_mSelf == nullptr) {
				_mSelf = std::make_shared<AssetManager>();
			}
			return _mSelf;
		}
	}
}
