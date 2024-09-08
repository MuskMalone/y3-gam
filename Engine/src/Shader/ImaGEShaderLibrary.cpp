#include "Shader/ImaGEShaderLibrary.h"


std::unordered_map<std::string, H2M::RefH2M<ImaGEShader>> ImaGEShaderLibrary::s_Shaders;


void ImaGEShaderLibrary::Add(H2M::RefH2M<ImaGEShader>& shader)
{
	auto name = shader->GetName();
	if (s_Shaders.find(name) == s_Shaders.end()) {
		s_Shaders[name] = shader;
	}
}

void ImaGEShaderLibrary::Load(const std::string& name, const std::string& vertexLocation, const std::string& fragmentLocation)
{
	auto shader = ImaGEShader::Create(vertexLocation.c_str(), fragmentLocation.c_str());
	if (s_Shaders.find(name) == s_Shaders.end()) {
		s_Shaders[name] = shader;
	}
}

void ImaGEShaderLibrary::Load(const std::string& name, const std::string& vertexLocation, const std::string& geometryLocation, const std::string& fragmentLocation)
{
	auto shader = ImaGEShader::Create(vertexLocation.c_str(), geometryLocation.c_str(), fragmentLocation.c_str());
	if (s_Shaders.find(name) == s_Shaders.end()) {
		s_Shaders[name] = shader;
	}
}

void ImaGEShaderLibrary::Load(const std::string& name, const std::string& computeLocation)
{
	auto shader = ImaGEShader::Create(computeLocation.c_str());
	if (s_Shaders.find(name) == s_Shaders.end()) {
		s_Shaders[name] = shader;
	}
}

const H2M::RefH2M<ImaGEShader>& ImaGEShaderLibrary::Get(const std::string& name)
{
	if (s_Shaders.find(name) != s_Shaders.end()) {
		return s_Shaders[name];
	}
	return H2M::RefH2M<ImaGEShader>();
}
