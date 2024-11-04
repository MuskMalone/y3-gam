#include <pch.h>
#include "PostProcessingManager.h"
#include <Asset/AssetUtils.h>
#include <Serialization/Serializer.h>
#include <Serialization/Deserializer.h>
#include <Scenes/SceneManager.h>
namespace Graphics {
	PostProcessingManager::PostProcessingManager() : 
		mDefaultShader{ std::make_shared<Graphics::Shader>(IGE::Assets::cDefaultVertShader, std::string(gAssetsDirectory) + "Shaders\\PostProcessingDefault.frag.glsl")}
	{
		CreateConfigFile();
		//Serialization::Deserializer::DeserializeAny(mConfigs, cConfigFilePath);
	}
	PostProcessingManager::~PostProcessingManager()
	{
		//Serialization::Serializer::SerializeAny(mConfigs, cConfigFilePath);
	}

	void PostProcessingManager::SetShader(IGE::Assets::GUID guid)
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (!name.empty()) {
			if (mConfigs.find(name) != mConfigs.end()) {
				if (mConfigs.at(name) == guid) return;
				else RemoveShader();
			}
			try {
				IGE_ASSETMGR.LoadRef<IGE::Assets::ShaderAsset>(guid);
				mConfigs.emplace(name, guid);
			}
			catch (...) {
				Debug::DebugLogger::GetInstance().LogError("failed to load post processing shader");
			}
		}
	}
	void PostProcessingManager::RemoveShader()
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (mConfigs.find(name) != mConfigs.end()) {
			//should unload shdr here
			mConfigs.erase(name);
		}
	}
	std::shared_ptr<Graphics::Shader> PostProcessingManager::GetShader()
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (name.empty() || mConfigs.find(name) == mConfigs.end()) {
			return mDefaultShader;
		}
		return IGE_ASSETMGR.GetAsset<IGE::Assets::ShaderAsset>(mConfigs.at(name))->mShader;
	}
	std::string PostProcessingManager::GetShaderName()
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (name.empty() || mConfigs.find(name) == mConfigs.end()) {
			return "";
		}
		return IGE_ASSETMGR.GUIDToPath(mConfigs.at(name));
	}
	void PostProcessingManager::ReloadShader()
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (mConfigs.find(name) != mConfigs.end()) {
			IGE_ASSETMGR.ReloadRef<IGE::Assets::ShaderAsset>(mConfigs.at(name));
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
}