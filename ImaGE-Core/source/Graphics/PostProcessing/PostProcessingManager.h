#pragma once
#include <Asset/Assetables/Shader/ShaderAsset.h>
#include <Singleton/ThreadSafeSingleton.h>
#include "Events/EventCallback.h"

namespace Graphics {
	class PostProcessingManager : public ThreadSafeSingleton<PostProcessingManager>{
	public:
		struct PostProcessingConfigs {
			std::unordered_map<std::string, uint64_t> mConfigs;
		};
	public:
		PostProcessingManager();
		~PostProcessingManager();
		void SetShader(IGE::Assets::GUID);
		void RemoveShader();
		std::shared_ptr<Graphics::Shader> GetShader();
		std::string GetShaderName();
		void ReloadShader();
	private:
		void CreateConfigFile();
	private: 
		//priority will be based off the 
		const std::string cConfigDirectory{ IGE::Assets::cPostProcessingDirectory + "Config\\"};
		const std::string cConfigFilePath{ cConfigDirectory + "postprocessing.igeconfig" };
		// stored in order. idx 0 will be the very first pass run on the framebuffer
		// idx 1 will be second ... idx n will be the n + 1th
		std::shared_ptr<Graphics::Shader> mDefaultShader;
		PostProcessingConfigs mPpc;
		EVENT_CALLBACK_DECL(HandleSystemEvents);
	};
}