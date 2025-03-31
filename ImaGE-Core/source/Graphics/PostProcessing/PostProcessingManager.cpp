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
		SUBSCRIBE_CLASS_FUNC(Events::SceneStateChange, &PostProcessingManager::HandleSystemEvents, this);

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
				mPpc.mConfigs.at(name).shaders.push_back(PostProcessingConfigs::ShaderLayer{true, guid});
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
			if (idx > 0 && idx < mPpc.mConfigs.at(name).shaders.size()) {
				std::swap(mPpc.mConfigs.at(name).shaders[idx], mPpc.mConfigs.at(name).shaders[idx - 1]);
			}
		}
	}
	void PostProcessingManager::MovePriorityDown(unsigned idx)
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (mPpc.mConfigs.find(name) != mPpc.mConfigs.end()) {
			//should unload shdr here
			if (mPpc.mConfigs.at(name).shaders.size() > 1 && idx < mPpc.mConfigs.at(name).shaders.size() - 1) {
				std::swap(mPpc.mConfigs.at(name).shaders[idx], mPpc.mConfigs.at(name).shaders[idx + 1]);
			}
		}
	}
	void PostProcessingManager::RemoveShader(unsigned idx)
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (mPpc.mConfigs.find(name) != mPpc.mConfigs.end()) {
			if (idx < mPpc.mConfigs.at(name).shaders.size()) {
				//delete the element at idx
				//should unload shdr here
				mPpc.mConfigs.at(name).shaders.erase(mPpc.mConfigs.at(name).shaders.begin() + idx);
			}
			if (mPpc.mConfigs.at(name).shaders.empty()) {
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
		if (idx >= mPpc.mConfigs.at(name).shaders.size()) return mDefaultShader;
		if (mPpc.mConfigs.at(name).shaders[idx].active)
			return IGE_ASSETMGR.GetAsset<IGE::Assets::ShaderAsset>(mPpc.mConfigs.at(name).shaders[idx].shader)->mShader;
		else return mDefaultShader;
		//return mDefaultShader;
	}

	void PostProcessingManager::SetShaderState(unsigned idx, bool active) {
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (name.empty() || mPpc.mConfigs.find(name) != mPpc.mConfigs.end()) {
			if (idx < mPpc.mConfigs.at(name).shaders.size()) {
				mPpc.mConfigs.at(name).shaders[idx].active = active;
			}
		}
	}

	std::shared_ptr<Graphics::Shader> PostProcessingManager::GetDefaultShader()
	{
		return mDefaultShader;
	}
	std::string PostProcessingManager::GetShaderName(unsigned idx)
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (name.empty() || mPpc.mConfigs.find(name) == mPpc.mConfigs.end()) {
			return "";
		}
		if (idx >= mPpc.mConfigs.at(name).shaders.size()) return "";
		return IGE_ASSETMGR.GUIDToPath(mPpc.mConfigs.at(name).shaders[idx].shader);
	}
	unsigned PostProcessingManager::GetShaderNum()
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (name.empty() || mPpc.mConfigs.find(name) == mPpc.mConfigs.end()) {
			return 0;
		}
		return static_cast<unsigned>(mPpc.mConfigs.at(name).shaders.size());
	}
	void PostProcessingManager::ReloadShaders()
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (mPpc.mConfigs.find(name) != mPpc.mConfigs.end()) {
			for (auto shaderlayer : mPpc.mConfigs.at(name).shaders) {
				IGE_ASSETMGR.ReloadRef<IGE::Assets::ShaderAsset>(shaderlayer.shader);
			}
		}

	}
	void PostProcessingManager::ReloadShader(unsigned idx)
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (mPpc.mConfigs.find(name) != mPpc.mConfigs.end()) {
			if (idx < mPpc.mConfigs.at(name).shaders.size()) {
				IGE_ASSETMGR.ReloadRef<IGE::Assets::ShaderAsset>(mPpc.mConfigs.at(name).shaders[idx].shader);
			}
		}
	}
	float& PostProcessingManager::GetFogMinDist()
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (mPpc.mConfigs.find(name) == mPpc.mConfigs.end()) {
			mPpc.mConfigs.emplace(name, PostProcessingConfigs::ShaderOrder{});
		}
		return mPpc.mConfigs.at(name).fogMinDist;
	}
	float& PostProcessingManager::GetFogMaxDist()
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (mPpc.mConfigs.find(name) == mPpc.mConfigs.end()) {
			mPpc.mConfigs.emplace(name, PostProcessingConfigs::ShaderOrder{});
		}
		return mPpc.mConfigs.at(name).fogMaxDist;
	}
	glm::vec3& PostProcessingManager::GetFogColor()
	{
		auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
		if (mPpc.mConfigs.find(name) == mPpc.mConfigs.end()) {
			mPpc.mConfigs.emplace(name, PostProcessingConfigs::ShaderOrder{});
		}
		return mPpc.mConfigs.at(name).fogColor;
	}
	void PostProcessingManager::SetShaderElapsedTime(float elapsedTime)
	{
		shaderElapsedTime = elapsedTime;
	}
	float PostProcessingManager::GetShaderElapsedTime()
	{
		return shaderElapsedTime;
	}
	void PostProcessingManager::SetStrength(float strength)
	{
		vigStrength = strength;
	}
	float PostProcessingManager::GetStrength()
	{
		return vigStrength;
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
		if (IGE_SCENEMGR.GetSceneState() == Scenes::PREFAB_EDITOR) { return; }

		auto const& state{ CAST_TO_EVENT(Events::SceneStateChange)->mNewState };

		if (state == Events::SceneStateChange::CHANGED) {
			auto name{ Scenes::SceneManager::GetInstance().GetSceneName() };
			if (mPpc.mConfigs.find(name) != mPpc.mConfigs.end()) {
				for (auto shaderlayer : mPpc.mConfigs.at(name).shaders) {
					LoadShader(shaderlayer.shader);
				}
			}
		}
	}
}