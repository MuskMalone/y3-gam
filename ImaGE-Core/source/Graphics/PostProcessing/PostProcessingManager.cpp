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

	void PostProcessingManager::SetShader(IGE::Assets::GUID guid)
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		//check if the name is empty
		if (!name.empty()) {
			//load the asset first before emplacing (to account for on change scene event)
			try {
				IGE_ASSETMGR.LoadRef<IGE::Assets::ShaderAsset>(guid);
				if (mPpc.mConfigs.find(name) != mPpc.mConfigs.end()) {
					if (IGE::Assets::GUID{ mPpc.mConfigs.at(name) } == guid) return;
					else RemoveShader();
				}

				mPpc.mConfigs.emplace(name, guid);
			}
			catch (...) {
				Debug::DebugLogger::GetInstance().LogError("failed to load post processing shader");
			}

		}
	}
	void PostProcessingManager::RemoveShader()
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (mPpc.mConfigs.find(name) != mPpc.mConfigs.end()) {
			//should unload shdr here
			mPpc.mConfigs.erase(name);
		}
	}
	std::shared_ptr<Graphics::Shader> PostProcessingManager::GetShader()
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (name.empty() || mPpc.mConfigs.find(name) == mPpc.mConfigs.end()) {
			return mDefaultShader;
		}
		return IGE_ASSETMGR.GetAsset<IGE::Assets::ShaderAsset>(mPpc.mConfigs.at(name))->mShader;
	}
	std::string PostProcessingManager::GetShaderName()
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (name.empty() || mPpc.mConfigs.find(name) == mPpc.mConfigs.end()) {
			return "";
		}
		return IGE_ASSETMGR.GUIDToPath(mPpc.mConfigs.at(name));
	}
	void PostProcessingManager::ReloadShader()
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (mPpc.mConfigs.find(name) != mPpc.mConfigs.end()) {
			IGE_ASSETMGR.ReloadRef<IGE::Assets::ShaderAsset>(mPpc.mConfigs.at(name));
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
				SetShader(mPpc.mConfigs.at(name));
			}
		}
	}
}