#include "Shader/ImaGEShader.h"

#include "H2M/Core/AssertH2M.h"
#include "H2M/Renderer/RendererAPI_H2M.h"

#include "Core/Log.h"
#include "Core/Util.h"
#include "Platform/OpenGL/OpenGLImaGEShader.h"
//#include "Platform/Vulkan/VulkanImaGEShader.h"
//#include "Platform/DX11/DX11Shader.h"


std::vector<H2M::RefH2M<ImaGEShader>> ImaGEShader::s_AllShaders;
ImaGEShaderSpecification ImaGEShader::s_Specification = ImaGEShaderSpecification{};


// the ultimate Create method that can create both ImaGEShader and HazelShader shader types
H2M::RefH2M<ImaGEShader> ImaGEShader::Create(ImaGEShaderSpecification imaGEShaderSpecification)
{
	s_Specification = imaGEShaderSpecification;

	H2M::RefH2M<ImaGEShader> imaGEShader;

	if (imaGEShaderSpecification.ShaderType == ImaGEShaderSpecification::ShaderType::ImaGEShader)
	{
		imaGEShader = OpenGLImaGEShader::Create(imaGEShaderSpecification.VertexShaderPath.c_str(), imaGEShaderSpecification.FragmentShaderPath.c_str(), imaGEShaderSpecification.ForceCompile);
	}
	else if (imaGEShaderSpecification.ShaderType == ImaGEShaderSpecification::ShaderType::HazelShader)
	{
		H2M::RefH2M<H2M::ShaderH2M> hazelShader = ShaderH2M::Create(imaGEShaderSpecification.HazelShaderPath, imaGEShaderSpecification.ForceCompile);
		imaGEShader = H2M::RefH2M<ImaGEShader>(hazelShader);
	}
	//else if (imaGEShaderSpecification.ShaderType == ImaGEShaderSpecification::ShaderType::DX11Shader)
	//{
	//	H2M::RefH2M<DX11Shader> dx11Shader = H2M::RefH2M<DX11Shader>::Create(
	//		Util::to_wstr(imaGEShaderSpecification.VertexShaderPath.c_str()).c_str(),
	//		Util::to_wstr(imaGEShaderSpecification.PixelShaderPath.c_str()).c_str());
	//	imaGEShader = H2M::RefH2M<ImaGEShader>(dx11Shader);
	//}

	return imaGEShader;
}

ImaGEShader::ImaGEShader()
{
	shaderID = 0;

	m_Name = "Untitled";

	m_UniformLocations = std::map<std::string, int>();
}

ImaGEShader::ImaGEShader(const char* vertexLocation, const char* fragmentLocation, bool forceCompile)
	: ImaGEShader()
{
	m_ShaderFilepath_Vertex = vertexLocation;
	m_ShaderFilepath_Fragment = fragmentLocation;

	m_Name = Util::StripExtensionFromFileName(Util::GetFileNameFromFullPath(vertexLocation));

	CreateFromFiles(vertexLocation, fragmentLocation);
}

ImaGEShader::ImaGEShader(const char* vertexLocation, const char* geometryLocation, const char* fragmentLocation, bool forceCompile)
	: ImaGEShader()
{
	m_ShaderFilepath_Vertex = vertexLocation;
	m_ShaderFilepath_Geometry = geometryLocation;
	m_ShaderFilepath_Fragment = fragmentLocation;

	m_Name = Util::StripExtensionFromFileName(Util::GetFileNameFromFullPath(vertexLocation));

	CreateFromFiles(vertexLocation, geometryLocation, fragmentLocation);
}

ImaGEShader::ImaGEShader(const char* computeLocation, bool forceCompile)
	: ImaGEShader()
{
	m_ShaderFilepath_Compute = computeLocation;

	m_Name = Util::StripExtensionFromFileName(Util::GetFileNameFromFullPath(computeLocation));

	CreateFromFileCompute(computeLocation);

	CompileProgram();
}

H2M::RefH2M<ImaGEShader> ImaGEShader::Create(const char* vertexLocation, const char* fragmentLocation, bool forceCompile)
{
	H2M::RefH2M<ImaGEShader> result = H2M::RefH2M<ImaGEShader>();

	switch (H2M::RendererAPI_H2M::Current())
	{
	case H2M::RendererAPITypeH2M::None: return H2M::RefH2M<ImaGEShader>();
	case H2M::RendererAPITypeH2M::OpenGL:
		result = H2M::RefH2M<OpenGLImaGEShader>::Create(vertexLocation, fragmentLocation, forceCompile);
		break;
	//case H2M::RendererAPITypeH2M::Vulkan:
	//	Log::GetLogger()->error("Not implemented for Vulkan API!");
	//	break;
	}
	s_AllShaders.push_back(result);
	return result;
}

H2M::RefH2M<ImaGEShader> ImaGEShader::Create(const char* vertexLocation, const char* geometryLocation, const char* fragmentLocation, bool forceCompile)
{
	H2M::RefH2M<ImaGEShader> result = H2M::RefH2M<ImaGEShader>();

	switch (H2M::RendererAPI_H2M::Current())
	{
	case H2M::RendererAPITypeH2M::None: return H2M::RefH2M<ImaGEShader>();
	case H2M::RendererAPITypeH2M::OpenGL:
		result = H2M::RefH2M<OpenGLImaGEShader>::Create(vertexLocation, geometryLocation, fragmentLocation, forceCompile);
		break;
	//case H2M::RendererAPITypeH2M::Vulkan:
	//	Log::GetLogger()->error("Not implemented for Vulkan API!");
	//	break;
	}
	s_AllShaders.push_back(result);
	return result;
}

H2M::RefH2M<ImaGEShader> ImaGEShader::Create(const char* computeLocation, bool forceCompile)
{
	H2M::RefH2M<ImaGEShader> result = H2M::RefH2M<ImaGEShader>();

	switch (H2M::RendererAPI_H2M::Current())
	{
	case H2M::RendererAPITypeH2M::None: return H2M::RefH2M<ImaGEShader>();
	case H2M::RendererAPITypeH2M::OpenGL:
		result = H2M::RefH2M<OpenGLImaGEShader>::Create(computeLocation, forceCompile);
		break;
	//case H2M::RendererAPITypeH2M::Vulkan:
	//	Log::GetLogger()->error("Not implemented for Vulkan API!");
	//	break;
	}
	s_AllShaders.push_back(result);
	return result;
}

void ImaGEShader::CreateFromString(const char* vertexCode, const char* fragmentCode)
{
	CompileShader(vertexCode, fragmentCode);
}

void ImaGEShader::CreateFromFiles(const char* vertexLocation, const char* fragmentLocation)
{
	std::string vertexString = ReadFile(vertexLocation);
	std::string fragmentString = ReadFile(fragmentLocation);
	const char* vertexCode = vertexString.c_str();
	const char* fragmentCode = fragmentString.c_str();
	CompileShader(vertexCode, fragmentCode);
}

void ImaGEShader::CreateFromFiles(const char* vertexLocation, const char* geometryLocation, const char* fragmentLocation)
{
	std::string vertexString = ReadFile(vertexLocation);
	std::string geometryString = ReadFile(geometryLocation);
	std::string fragmentString = ReadFile(fragmentLocation);
	const char* vertexCode = vertexString.c_str();
	const char* geometryCode = geometryString.c_str();
	const char* fragmentCode = fragmentString.c_str();
	CompileShader(vertexCode, geometryCode, fragmentCode);
}

void ImaGEShader::CreateFromFileVertex(const char* vertexLocation)
{
	std::string vertexString = ReadFile(vertexLocation);
	const char* vertexCode = vertexString.c_str();
	AddShaderVertex(vertexCode);
}

void ImaGEShader::CreateFromFileFragment(const char* fragmentLocation)
{
	std::string fragmentString = ReadFile(fragmentLocation);
	const char* fragmentCode = fragmentString.c_str();
	AddShaderFragment(fragmentCode);
}

void ImaGEShader::CreateFromFileGeometry(const char* geometryLocation)
{
	std::string geometryString = ReadFile(geometryLocation);
	const char* geometryCode = geometryString.c_str();
	AddShaderGeometry(geometryCode);
}

void ImaGEShader::CreateFromFileCompute(const char* computeLocation)
{
	std::string computeString = ReadFile(computeLocation);
	const char* computeCode = computeString.c_str();
	AddShaderCompute(computeCode);
}

std::string ImaGEShader::ReadFile(const char* fileLocation)
{
	std::string content;
	std::ifstream fileStream(fileLocation, std::ios::in);

	if (!fileStream.is_open())
	{
		LOG_ERROR("Failed to read '{0}'! File doesn't exist.", fileLocation);
		return "";
	}

	std::string line = "";
	while (!fileStream.eof())
	{
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();

	LOG_INFO("Content loaded from file '{0}'", fileLocation);

	return content;
}

void ImaGEShader::Validate()
{
	if (m_Validated) return;

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glValidateProgram(programID);
	glGetProgramiv(programID, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(programID, sizeof(eLog), NULL, eLog);
		LOG_ERROR("Shader program [ID={0}] validation error: '{1}'", programID, eLog);
		return;
	}

	LOG_INFO("Shader program [ID={0}] validation complete.", programID);

	m_Validated = true;
}

GLuint ImaGEShader::GetProgramID()
{
	return programID;
}

GLint ImaGEShader::GetUniformLocation(const std::string& name)
{
	std::map<std::string, int>::const_iterator it;
	it = m_UniformLocations.find(name);

	if (it != m_UniformLocations.end()) {
		return it->second;
	} else {
		glUseProgram(programID);
		int uniformLocation = glGetUniformLocation(programID, name.c_str());
		if (uniformLocation != -1) {
			m_UniformLocations.insert(std::make_pair(name, uniformLocation));
		}
		return uniformLocation;
	}
}

const std::unordered_map<std::string, H2M::ShaderBufferH2M>& ImaGEShader::GetShaderBuffers() const
{
	// OpenGLMaterial::FindUniformDeclaration requires at least 2 shader buffers
	// std::unordered_map<std::string, H2M::ShaderBuffer> shaderBuffers = ;
	// shaderBuffers.insert(std::make_pair("One", H2M::ShaderBuffer()));
	// shaderBuffers.insert(std::make_pair("Two", H2M::ShaderBuffer()));
	return std::unordered_map<std::string, H2M::ShaderBufferH2M>();
}

const std::unordered_map<std::string, H2M::ShaderResourceDeclarationH2M>& ImaGEShader::GetResources() const
{
	return std::unordered_map<std::string, H2M::ShaderResourceDeclarationH2M>();
}

void ImaGEShader::AddShaderReloadedCallback(const ShaderReloadedCallback& callback)
{
}

uint32_t ImaGEShader::GetRendererID() const
{
	return programID;
}

size_t ImaGEShader::GetHash() const
{
	return std::hash<std::string>{}(m_ShaderFilepath_Vertex + m_ShaderFilepath_Compute + m_ShaderFilepath_Fragment);
}

void ImaGEShader::Bind()
{
	Log::GetLogger()->error("ImaGEShader::Bind() shouldn't be called directly!");
}

void ImaGEShader::Reload(bool forceCompile)
{
	Log::GetLogger()->error("ImaGEShader::Reload() shouldn't be called directly!");
}

void ImaGEShader::SetUniformBuffer(const std::string& name, const void* data, uint32_t size)
{
	Log::GetLogger()->error("ImaGEShader::SetUniformBuffer() shouldn't be called directly!");
}

void ImaGEShader::SetUniform(const std::string& fullname, float value)
{
	Log::GetLogger()->error("ImaGEShader::SetUniform() shouldn't be called directly!");
}

void ImaGEShader::SetUniform(const std::string& fullname, uint32_t value)
{
	Log::GetLogger()->error("ImaGEShader::SetUniform() shouldn't be called directly!");
}

void ImaGEShader::SetUniform(const std::string& fullname, int value)
{
	Log::GetLogger()->error("ImaGEShader::SetUniform() shouldn't be called directly!");
}

void ImaGEShader::SetUniform(const std::string& fullname, const glm::vec2& value)
{
	Log::GetLogger()->error("ImaGEShader::SetUniform() shouldn't be called directly!");
}

void ImaGEShader::SetUniform(const std::string& fullname, const glm::vec3& value)
{
	Log::GetLogger()->error("ImaGEShader::SetUniform() shouldn't be called directly!");
}

void ImaGEShader::SetUniform(const std::string& fullname, const glm::vec4& value)
{
	Log::GetLogger()->error("ImaGEShader::SetUniform() shouldn't be called directly!");
}

void ImaGEShader::SetUniform(const std::string& fullname, const glm::mat3& value)
{
	Log::GetLogger()->error("ImaGEShader::SetUniform() shouldn't be called directly!");
}

void ImaGEShader::SetUniform(const std::string& fullname, const glm::mat4& value)
{
	Log::GetLogger()->error("ImaGEShader::SetUniform() shouldn't be called directly!");
}

void ImaGEShader::SetFloat(const std::string& name, float value)
{
	Log::GetLogger()->error("ImaGEShader::SetFloat() shouldn't be called directly!");
}

void ImaGEShader::SetUInt(const std::string& name, uint32_t value)
{
	Log::GetLogger()->error("ImaGEShader::SetUInt() shouldn't be called directly!");
}

void ImaGEShader::SetInt(const std::string& name, int value)
{
	Log::GetLogger()->error("ImaGEShader::SetInt() shouldn't be called directly!");
}

void ImaGEShader::SetBool(const std::string& name, bool value)
{
	Log::GetLogger()->error("ImaGEShader::SetBool() shouldn't be called directly!");
}

void ImaGEShader::SetFloat2(const std::string& name, const glm::vec2& value)
{
	Log::GetLogger()->error("ImaGEShader::SetFloat2() shouldn't be called directly!");
}

void ImaGEShader::SetFloat3(const std::string& name, const glm::vec3& value)
{
	Log::GetLogger()->error("ImaGEShader::SetFloat3() shouldn't be called directly!");
}

void ImaGEShader::SetFloat4(const std::string& name, const glm::vec4& value)
{
	Log::GetLogger()->error("ImaGEShader::SetFloat4() shouldn't be called directly!");
}

void ImaGEShader::SetMat2(const std::string& name, const glm::mat2& mat)
{
	Log::GetLogger()->error("ImaGEShader::SetMat2() shouldn't be called directly!");
}

void ImaGEShader::SetMat3(const std::string& name, const glm::mat3& mat)
{
	Log::GetLogger()->error("ImaGEShader::SetMat3() shouldn't be called directly!");
}

void ImaGEShader::SetMat4(const std::string& name, const glm::mat4& value)
{
	Log::GetLogger()->error("ImaGEShader::SetMat4() shouldn't be called directly!");
}

void ImaGEShader::SetMat4FromRenderThread(const std::string& name, const glm::mat4& value, bool bind)
{
	Log::GetLogger()->error("ImaGEShader::SetMat4FromRenderThread() shouldn't be called directly!");
}

void ImaGEShader::SetIntArray(const std::string& name, int* values, uint32_t size)
{
	Log::GetLogger()->error("ImaGEShader::SetIntArray() shouldn't be called directly!");
}

// Remove and replace with SetFloat2
void ImaGEShader::SetVec2(const std::string& name, const glm::vec2& value)
{
	Log::GetLogger()->error("ImaGEShader::SetVec2() shouldn't be called directly!");
}

// Remove and replace with SetFloat2
void ImaGEShader::SetVec2(const std::string& name, float x, float y)
{
	Log::GetLogger()->error("ImaGEShader::SetVec2() shouldn't be called directly!");
}

void ImaGEShader::UploadUniformMat4(const std::string& name, const glm::mat4& values)
{
	Log::GetLogger()->error("ImaGEShader::UploadUniformMat4() shouldn't be called directly!");
}

void ImaGEShader::UploadUniformMat4(uint32_t location, const glm::mat4& values)
{
	Log::GetLogger()->error("ImaGEShader::UploadUniformMat4() shouldn't be called directly!");
}

void ImaGEShader::setLightMat4(std::vector<glm::mat4> lightMatrices)
{
	Log::GetLogger()->error("ImaGEShader::setLightMat4() shouldn't be called directly!");
}

void ImaGEShader::SetLightMatrices(std::vector<glm::mat4> lightMatrices)
{
	Log::GetLogger()->error("ImaGEShader::SetLightMatrices() shouldn't be called directly!");
}

void ImaGEShader::Unbind()
{
	Log::GetLogger()->error("ImaGEShader::Unbind() shouldn't be called directly!");
}

ImaGEShader::~ImaGEShader()
{
	ClearShader();
}

void ImaGEShader::ClearShader()
{
}

void ImaGEShader::CompileShader(const char* vertexCode, const char* fragmentCode)
{
	programID = glCreateProgram();

	if (!programID)
	{
		LOG_ERROR("Error creating shader program!");
		return;
	}

	AddShader(programID, vertexCode, GL_VERTEX_SHADER);
	AddShader(programID, fragmentCode, GL_FRAGMENT_SHADER);

	CompileProgram();
}

void ImaGEShader::CompileShader(const char* vertexCode, const char* geometryCode, const char* fragmentCode)
{
	programID = glCreateProgram();

	if (!programID)
	{
		LOG_ERROR("Error creating shader program!");
		return;
	}

	AddShader(programID, vertexCode,   GL_VERTEX_SHADER);
	AddShader(programID, geometryCode, GL_GEOMETRY_SHADER);
	AddShader(programID, fragmentCode, GL_FRAGMENT_SHADER);

	CompileProgram();
}

void ImaGEShader::AddShaderVertex(const char* vertexCode)
{
	if (programID == -1) {
		programID = glCreateProgram();
	}

	if (!programID)
	{
		LOG_ERROR("Program ID not valid (VERTEX shader)!");
		return;
	}

	AddShader(programID, vertexCode, GL_VERTEX_SHADER);
}

void ImaGEShader::AddShaderFragment(const char* fragmentCode)
{
	if (programID == -1) {
		programID = glCreateProgram();
	}

	if (!programID)
	{
		LOG_ERROR("Program ID not valid (FRAGMENT shader)!");
		return;
	}

	AddShader(programID, fragmentCode, GL_FRAGMENT_SHADER);
}

void ImaGEShader::AddShaderGeometry(const char* geometryCode)
{
	if (programID == -1) {
		programID = glCreateProgram();
	}

	if (!programID)
	{
		LOG_ERROR("Program ID not valid (GEOMETRY shader)!");
		return;
	}

	AddShader(programID, geometryCode, GL_GEOMETRY_SHADER);
}

void ImaGEShader::AddShaderCompute(const char* computeCode)
{
	if (programID == -1) {
		programID = glCreateProgram();
	}

	if (!programID)
	{
		LOG_ERROR("Program ID not valid (COMPUTE shader)!");
		return;
	}

	AddShader(programID, computeCode, GL_COMPUTE_SHADER);
}

const char* ImaGEShader::GetShaderTypeNameFromEnum(const GLenum shaderType)
{
	const char* shaderTypeName = "Unknown";
	if (shaderType == GL_VERTEX_SHADER)               shaderTypeName = "Vertex";
	else if (shaderType == GL_FRAGMENT_SHADER)        shaderTypeName = "Fragment";
	else if (shaderType == GL_TESS_CONTROL_SHADER)    shaderTypeName = "Tessellation Control";
	else if (shaderType == GL_TESS_EVALUATION_SHADER) shaderTypeName = "Tessellation Evaluation";
	else if (shaderType == GL_GEOMETRY_SHADER)        shaderTypeName = "Geometry";
	else if (shaderType == GL_COMPUTE_SHADER)         shaderTypeName = "Compute";
	return shaderTypeName;
}

void ImaGEShader::AddShader(GLuint programID, const char* shaderCode, GLenum shaderType)
{
	const char* shaderTypeName = GetShaderTypeNameFromEnum(shaderType);

	GLuint shaderID = glCreateShader(shaderType);

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = (GLint)strlen(shaderCode);

	glShaderSource(shaderID, 1, theCode, codeLength);
	glCompileShader(shaderID);

	// Log::GetLogger()->info("-- BEGIN shader code --");
	// Log::GetLogger()->info("{0}", shaderCode);
	// Log::GetLogger()->info("-- END shader code --");

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		LOG_ERROR("{0} shader compilation error: '{1}'", shaderTypeName, eLog);
		return;
	}

	LOG_INFO("{0} shader compiled.", shaderTypeName);

	glAttachShader(programID, shaderID);
	return;
}

void ImaGEShader::CompileProgram()
{
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(programID, sizeof(eLog), NULL, eLog);
		LOG_ERROR("Shader program linking error: '{0}'", eLog);
		
		return;
	}

	LOG_INFO("Shader program linking complete.");

	GetUniformLocations();
}

void ImaGEShader::GetUniformLocations()
{
	for (unsigned int i = 0; i < 6; i++)
	{
		char locBuff[100] = { '\0' };

		snprintf(locBuff, sizeof(locBuff), "lightMatrices[%d]", i);
		uniformLightMatrices[i] = glGetUniformLocation(programID, locBuff);
	}
}
