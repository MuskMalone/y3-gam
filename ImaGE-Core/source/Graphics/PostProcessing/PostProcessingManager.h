#pragma once
#include <Asset/Assetables/Shader/ShaderAsset.h>
#include <Singleton/ThreadSafeSingleton.h>
#include "Events/EventCallback.h"

namespace Graphics {
	class PostProcessingManager : public ThreadSafeSingleton<PostProcessingManager>{
	public:
		struct PostProcessingConfigs {
			struct ShaderLayer {
				bool active{ true };
				uint64_t shader;
			};
			struct ShaderOrder {
				float fogMinDist{1.f};
				float fogMaxDist{1000.f};
				glm::vec3 fogColor{1,1,1};
				std::vector<ShaderLayer> shaders;
			};
			// CE: changed to map so its always serialized the same order
			std::map<std::string, ShaderOrder> mConfigs;
			
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
		void SetShaderState(unsigned idx, bool active);
		std::shared_ptr<Graphics::Shader> GetDefaultShader();
		std::string GetShaderName(unsigned idx);
		unsigned GetShaderNum();
		void ReloadShaders();
		void ReloadShader(unsigned idx);
		float& GetFogMinDist();
		float& GetFogMaxDist();
		glm::vec3& GetFogColor();
		void SetShaderElapsedTime(float elapsedTime);
		float GetShaderElapsedTime();
		void SetStrength(float strength);
		float GetStrength();
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
		float shaderElapsedTime = 0; // Small workaround
		float vigStrength = 0;
		EVENT_CALLBACK_DECL(HandleSystemEvents);
	};
}