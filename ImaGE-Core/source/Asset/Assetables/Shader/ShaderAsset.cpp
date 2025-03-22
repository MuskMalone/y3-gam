#include <pch.h>
#include "ShaderAsset.h"
#include <Graphics/Shader.h>
namespace IGE {
	namespace Assets {
		ShaderAsset::ShaderAsset(std::string const& fp) : 
			mShader{ std::make_shared<Graphics::Shader>(cDefaultVertShader, fp) }
		{
		}
		IGE::Assets::GUID ShaderAsset::Import(std::string const& fp, std::string& newFp, AssetMetadata::AssetProps& metadata)
		{
			// Copy the image file to the assets folder
			std::string const inputShaderPath{ cPostProcessingDirectory + GetFileNameWithExtension(fp) };//GetAbsolutePath(cAudioDirectory + filename + fileext) };
			CreateDirectoryIfNotExists(cPostProcessingDirectory);

			if (!CopyFileToAssets(fp, inputShaderPath)) {
				return GUID{};
			}

			//populate the metatata//populate metadata (right now its only path)
			newFp = inputShaderPath;
			metadata.metadata["path"] = newFp;

			return GUID{ GUID::Seed{} };
		}
		void* ShaderAsset::Load(GUID guid)
		{
			//compilation is not needed
			//todo: maybe importing means creating a new text file containing the guids to different shaders
			std::string const& fp { AssetManager::GetInstance().GUIDToPath(guid) };
			return reinterpret_cast<void*>(new ShaderAsset(fp));
		}
		void ShaderAsset::Unload(ShaderAsset* ptr, GUID guid)
		{
			delete ptr;
		}
	}
}