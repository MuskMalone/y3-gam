#pragma once
#include <Asset/SmartPointer.h>
#include <Asset/AssetManager.h>
#include <Graphics/Shader.h>
namespace IGE{
	namespace Assets {
		const std::string cDefaultVertShader { std::string(gAssetsDirectory) + "Shaders\\PostProcessingDefault.vert.glsl" };
		const std::string cPostProcessingDirectory{ std::string(gAssetsDirectory) + "PostProcessing\\" };
		//this is assuming post processing only involves frag shaders (only one file)
		struct ShaderAsset : RefCounted{
			//this is a sharedptr to make it compatible with PipelineSpec's shader member;
			std::shared_ptr<Graphics::Shader> mShader;
			ShaderAsset(std::string const& fp);
			static IGE::Assets::GUID Import(std::string const& fp, std::string& newFp,
					AssetMetadata::AssetProps&);
			static void* Load([[maybe_unused]] GUID guid);
			static void Unload([[maybe_unused]] ShaderAsset* ptr, GUID guid);
		};
	}
}