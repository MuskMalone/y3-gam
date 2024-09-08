#pragma once

#include "H2M/Core/BaseH2M.h"

#include "Shader/ImaGEShader.h"

#include <unordered_map>


class ImaGEShaderLibrary
{
public:
	static void Add(H2M::RefH2M<ImaGEShader>& shader);
	static void Load(const std::string& name, const std::string& vertexLocation, const std::string& fragmentLocation);
	static void Load(const std::string& name, const std::string& vertexLocation, const std::string& geometryLocation, const std::string& fragmentLocation);
	static void Load(const std::string& name, const std::string& computeLocation);
	static const H2M::RefH2M<ImaGEShader>& Get(const std::string& name);

private:
	static std::unordered_map<std::string, H2M::RefH2M<ImaGEShader>> s_Shaders;

};
