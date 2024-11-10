#include <pch.h>
#include "PostProcessingManager.h"
#include <Asset/AssetUtils.h>
#include <Serialization/Serializer.h>
#include <Serialization/Deserializer.h>
#include <Scenes/SceneManager.h>
#include <Events/EventManager.h>
namespace Graphics {
	PostProcessingManager::PostProcessingManager() : 
		mDefaultShader{ std::make_shared<Graphics::Shader>(IGE::Assets::cDefaultVertShader, std::string(gAssetsDirectory) + "Shaders\\PostProcessingDefault.frag.glsl")}
	{
		SUBSCRIBE_CLASS_FUNC(Events::EventType::SCENE_STATE_CHANGE, &PostProcessingManager::HandleSystemEvents, this);

		CreateConfigFile();
		Serialization::Deserializer::DeserializeAny(mPpc, cConfigFilePath);
	}
	PostProcessingManager::~PostProcessingManager()
	{
		Serialization::Serializer::SerializeAny(mPpc, cConfigFilePath);
	}

	void PostProcessingManager::LoadShader(IGE::Assets::GUID guid)
	{
		IGE_ASSETMGR.LoadRef<IGE::Assets::ShaderAsset>(guid);
	}

	void PostProcessingManager::SetShader(IGE::Assets::GUID guid)
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		//check if the name is empty
		if (!name.empty()) {
			//load the asset first before emplacing (to account for on change scene event)
			try {
				LoadShader(guid);
				//if name is not found, create a vector
				if (mPpc.mConfigs.find(name) == mPpc.mConfigs.end()) {
					mPpc.mConfigs.emplace(name, PostProcessingConfigs::ShaderOrder{});
				}
				mPpc.mConfigs.at(name).push_back(guid);
			}
			catch (...) {
				Debug::DebugLogger::GetInstance().LogError("failed to load post processing shader");
			}

		}
	}
	void PostProcessingManager::MovePriorityUp(unsigned idx)
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (mPpc.mConfigs.find(name) != mPpc.mConfigs.end()) {
			//should unload shdr here
			if (idx > 0 && idx < mPpc.mConfigs.at(name).size()) {
				std::swap(mPpc.mConfigs.at(name)[idx], mPpc.mConfigs.at(name)[idx - 1]);
			}
		}
	}
	void PostProcessingManager::MovePriorityDown(unsigned idx)
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (mPpc.mConfigs.find(name) != mPpc.mConfigs.end()) {
			//should unload shdr here
			if (mPpc.mConfigs.at(name).size() > 1 && idx < mPpc.mConfigs.at(name).size() - 1) {
				std::swap(mPpc.mConfigs.at(name)[idx], mPpc.mConfigs.at(name)[idx + 1]);
			}
		}
	}
	void PostProcessingManager::RemoveShader(unsigned idx)
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (mPpc.mConfigs.find(name) != mPpc.mConfigs.end()) {
			if (idx < mPpc.mConfigs.at(name).size()) {
				//delete the element at idx
				//should unload shdr here
				mPpc.mConfigs.at(name).erase(mPpc.mConfigs.at(name).begin() + idx);
			}
			if (mPpc.mConfigs.at(name).empty()) {
				mPpc.mConfigs.erase(name);
			}
		}
	}
	std::shared_ptr<Graphics::Shader> PostProcessingManager::GetShader(unsigned idx)
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (name.empty() || mPpc.mConfigs.find(name) == mPpc.mConfigs.end()) {
			return mDefaultShader;
		}
		if (idx >= mPpc.mConfigs.at(name).size()) return mDefaultShader;
		return IGE_ASSETMGR.GetAsset<IGE::Assets::ShaderAsset>(mPpc.mConfigs.at(name)[idx])->mShader;
	}
	std::string PostProcessingManager::GetShaderName(unsigned idx)
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (name.empty() || mPpc.mConfigs.find(name) == mPpc.mConfigs.end()) {
			return "";
		}
		if (idx >= mPpc.mConfigs.at(name).size()) return "";
		return IGE_ASSETMGR.GUIDToPath(mPpc.mConfigs.at(name)[idx]);
	}
	unsigned PostProcessingManager::GetShaderNum()
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (name.empty() || mPpc.mConfigs.find(name) == mPpc.mConfigs.end()) {
			return 0;
		}
		return static_cast<unsigned>(mPpc.mConfigs.at(name).size());
	}
	void PostProcessingManager::ReloadShaders()
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (mPpc.mConfigs.find(name) != mPpc.mConfigs.end()) {
			for (auto guid : mPpc.mConfigs.at(name)) {
				IGE_ASSETMGR.ReloadRef<IGE::Assets::ShaderAsset>(guid);
			}
		}

	}
	void PostProcessingManager::ReloadShader(unsigned idx)
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (mPpc.mConfigs.find(name) != mPpc.mConfigs.end()) {
			if (idx < mPpc.mConfigs.at(name).size()) {
				IGE_ASSETMGR.ReloadRef<IGE::Assets::ShaderAsset>(mPpc.mConfigs.at(name)[idx]);
			}
		}
	}
	void PostProcessingManager::CreateConfigFile()
	{
		IGE::Assets::CreateDirectoryIfNotExists(IGE::Assets::cPostProcessingDirectory);
		IGE::Assets::CreateDirectoryIfNotExists(cConfigDirectory);
		if (!IGE::Assets::IsValidFilePath(cConfigFilePath)) {
			std::ofstream file(cConfigFilePath);
			file.close();
		}
	}

	EVENT_CALLBACK_DEF(PostProcessingManager, HandleSystemEvents) {
		auto const& state{ CAST_TO_EVENT(Events::SceneStateChange)->mNewState };
		if (state == Events::SceneStateChange::CHANGED) {
			auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
			if (mPpc.mConfigs.find(name) != mPpc.mConfigs.end()) {
				for (auto guid : mPpc.mConfigs.at(name)) {
					LoadShader(guid);
				}
			}
		}
	}
}