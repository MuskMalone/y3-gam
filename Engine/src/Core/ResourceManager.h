#pragma once

#include "H2M/Renderer/TextureH2M.h"

#include "Material/Material.h"
#include "Texture/ImaGETexture.h"

#include <map>


class ResourceManager
{
public:
	static void Init();
	static void LoadTexture(std::string name, std::string filePath);
	static void LoadTexture(std::string name, std::string filePath, GLenum filter, bool force);
	static void LoadMaterial(std::string name, TextureInfo textureInfo);
	static H2M::RefH2M<ImaGETexture> HotLoadTexture(std::string textureName);
	static H2M::RefH2M<Material> HotLoadMaterial(std::string materialName);

	// Getters
	static inline H2M::RefH2M<ImaGETexture> GetTexture(std::string textureName) { return s_Textures[textureName]; };
	static inline std::map<std::string, H2M::RefH2M<ImaGETexture>>* GetTextures() { return &s_Textures; };
	static inline std::map<std::string, H2M::RefH2M<Material>>* GetMaterials() { return &s_Materials; };
	static inline std::map<std::string, std::string>* GetTextureInfo() { return &s_TextureInfo; };
	static inline std::map<std::string, TextureInfo>* GetMaterialInfo() { return &s_MaterialInfo; };
	static inline std::map<std::string, H2M::RefH2M<ImaGEShader>>* GetShaders() { return &s_ShaderCacheByTitle; };

	// Loading Texture2D_H2M
	static H2M::RefH2M<H2M::Texture2D_H2M> LoadTexture2D_H2M(std::string filePath, bool sRGB);

	static void AddShader(std::string name, H2M::RefH2M<ImaGEShader> shader);
	static const H2M::RefH2M<ImaGEShader>& GetShader(std::string name);

	// Caching shaders
	static const H2M::RefH2M<ImaGEShader>& CreateOrLoadShader(ImaGEShaderSpecification imaGEShaderSpecification);

public:
	static float s_MaterialSpecular;
	static float s_MaterialShininess;

private:
	// Asset loading
	static std::map<std::string, std::string> s_TextureInfo;
	static std::map<std::string, TextureInfo> s_MaterialInfo;

	static std::map<std::string, H2M::RefH2M<ImaGETexture>> s_Textures;
	static std::map<std::string, H2M::RefH2M<Material>> s_Materials;

	static std::map<std::string, H2M::RefH2M<H2M::Texture2D_H2M>> s_HazelTextures2D;

	static std::map<std::string, H2M::RefH2M<ImaGEShader>> s_ShaderCacheByTitle;

	static std::map<std::string, H2M::RefH2M<ImaGEShader>> s_ShadersCacheByFilepath;

};
