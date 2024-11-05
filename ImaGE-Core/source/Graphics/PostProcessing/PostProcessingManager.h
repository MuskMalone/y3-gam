#pragma once
#include <Asset/Assetables/Shader/ShaderAsset.h>
#include <Singleton/ThreadSafeSingleton.h>
#include "Events/EventCallback.h"

namespace Graphics {
	class PostProcessingManager : public ThreadSafeSingleton<PostProcessingManager>{
	public:
		struct PostProcessingConfigs {
			using ShaderOrder = std::vector<uint64_t>;
			std::unordered_map<std::string, std::vector<uint64_t>> mConfigs;
		};
	public:
		PostProcessingManager();
		~PostProcessingManager();

		void LoadShader(IGE::Assets::GUID);
		//push in from the back
		void SetShader(IGE::Assets::GUID);
		void MovePriorityUp(unsigned idx);
		void MovePriorityDown(unsigned idx);
		void RemoveShader(unsigned idx);
		std::shared_ptr<Graphics::Shader> GetShader(unsigned idx);
		std::string GetShaderName(unsigned idx);
		unsigned GetShaderNum();
		void ReloadShaders();
		void ReloadShader(unsigned idx);
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