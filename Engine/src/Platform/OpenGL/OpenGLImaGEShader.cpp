#include "OpenGLImaGEShader.h"

#include "H2M/Core/AssertH2M.h"

#include "Core/Log.h"
#include "Core/Util.h"


OpenGLImaGEShader::OpenGLImaGEShader()
{
	shaderID = 0;

	m_Name = "Untitled";

	m_UniformLocations = std::map<std::string, int>();
}

OpenGLImaGEShader::OpenGLImaGEShader(const char* vertexLocation, const char* fragmentLocation, bool forceCompile)
	: OpenGLImaGEShader()
{
	m_ShaderFilepath_Vertex = vertexLocation;
	m_ShaderFilepath_Fragment = fragmentLocation;

	m_Name = Util::StripExtensionFromFileName(Util::GetFileNameFromFullPath(vertexLocation));

	CreateFromFiles(vertexLocation, fragmentLocation);
}

OpenGLImaGEShader::OpenGLImaGEShader(const char* vertexLocation, const char* geometryLocation, const char* fragmentLocation, bool forceCompile)
	: OpenGLImaGEShader()
{
	m_ShaderFilepath_Vertex = vertexLocation;
	m_ShaderFilepath_Geometry = geometryLocation;
	m_ShaderFilepath_Fragment = fragmentLocation;

	m_Name = Util::StripExtensionFromFileName(Util::GetFileNameFromFullPath(vertexLocation));

	CreateFromFiles(vertexLocation, geometryLocation, fragmentLocation);
}

OpenGLImaGEShader::OpenGLImaGEShader(const char* computeLocation, bool forceCompile)
	: OpenGLImaGEShader()
{
	m_ShaderFilepath_Compute = computeLocation;

	m_Name = Util::StripExtensionFromFileName(Util::GetFileNameFromFullPath(computeLocation));

	CreateFromFileCompute(computeLocation);
	CompileProgram();
}

H2M::RefH2M<OpenGLImaGEShader> OpenGLImaGEShader::Create(const char* vertexLocation, const char* fragmentLocation, bool forceCompile)
{
	H2M::RefH2M<OpenGLImaGEShader> result = H2M::RefH2M<OpenGLImaGEShader>::Create(vertexLocation, fragmentLocation, forceCompile);

	s_AllShaders.push_back(result);

	return result;
}

H2M::RefH2M<OpenGLImaGEShader> OpenGLImaGEShader::Create(const char* vertexLocation, const char* geometryLocation, const char* fragmentLocation, bool forceCompile)
{
	H2M::RefH2M<OpenGLImaGEShader> result = H2M::RefH2M<OpenGLImaGEShader>::Create(vertexLocation, geometryLocation, fragmentLocation, forceCompile);

	s_AllShaders.push_back(result);
	return result;
}

H2M::RefH2M<OpenGLImaGEShader> OpenGLImaGEShader::Create(const char* computeLocation, bool forceCompile)
{
	H2M::RefH2M<OpenGLImaGEShader> result = H2M::RefH2M<OpenGLImaGEShader>::Create(computeLocation, forceCompile);

	s_AllShaders.push_back(result);
	return result;
}

void OpenGLImaGEShader::CreateFromString(const char* vertexCode, const char* fragmentCode)
{
	CompileShader(vertexCode, fragmentCode);
}

void OpenGLImaGEShader::CreateFromFiles(const char* vertexLocation, const char* fragmentLocation)
{
	std::string vertexString = ReadFile(vertexLocation);
	std::string fragmentString = ReadFile(fragmentLocation);
	const char* vertexCode = vertexString.c_str();
	const char* fragmentCode = fragmentString.c_str();
	CompileShader(vertexCode, fragmentCode);
}

void OpenGLImaGEShader::CreateFromFiles(const char* vertexLocation, const char* geometryLocation, const char* fragmentLocation)
{
	std::string vertexString = ReadFile(vertexLocation);
	std::string geometryString = ReadFile(geometryLocation);
	std::string fragmentString = ReadFile(fragmentLocation);
	const char* vertexCode = vertexString.c_str();
	const char* geometryCode = geometryString.c_str();
	const char* fragmentCode = fragmentString.c_str();
	CompileShader(vertexCode, geometryCode, fragmentCode);
}

void OpenGLImaGEShader::CreateFromFileVertex(const char* vertexLocation)
{
	std::string vertexString = ReadFile(vertexLocation);
	const char* vertexCode = vertexString.c_str();
	AddShaderVertex(vertexCode);
}

void OpenGLImaGEShader::CreateFromFileFragment(const char* fragmentLocation)
{
	std::string fragmentString = ReadFile(fragmentLocation);
	const char* fragmentCode = fragmentString.c_str();
	AddShaderFragment(fragmentCode);
}

void OpenGLImaGEShader::CreateFromFileGeometry(const char* geometryLocation)
{
	std::string geometryString = ReadFile(geometryLocation);
	const char* geometryCode = geometryString.c_str();
	AddShaderGeometry(geometryCode);
}

void OpenGLImaGEShader::CreateFromFileCompute(const char* computeLocation)
{
	std::string computeString = ReadFile(computeLocation);
	const char* computeCode = computeString.c_str();
	AddShaderCompute(computeCode);
}

std::string OpenGLImaGEShader::ReadFile(const char* fileLocation)
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

void OpenGLImaGEShader::Validate()
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

GLuint OpenGLImaGEShader::GetProgramID()
{
	return programID;
}

GLint OpenGLImaGEShader::GetUniformLocation(const std::string& name)
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

void OpenGLImaGEShader::SetIntArray(const std::string& name, int* values, uint32_t size)
{
	glUniform1iv(GetUniformLocation(name), size, values);
}

const std::unordered_map<std::string, H2M::ShaderBufferH2M>& OpenGLImaGEShader::GetShaderBuffers() const
{
	// OpenGLMaterial::FindUniformDeclaration requires at least 2 shader buffers
	// std::unordered_map<std::string, H2M::ShaderBuffer> shaderBuffers = std::unordered_map<std::string, H2M::ShaderBuffer>();
	// shaderBuffers.insert(std::make_pair("One", H2M::ShaderBuffer()));
	// shaderBuffers.insert(std::make_pair("Two", H2M::ShaderBuffer()));
	return std::unordered_map<std::string, H2M::ShaderBufferH2M>();
}

const std::unordered_map<std::string, H2M::ShaderResourceDeclarationH2M>& OpenGLImaGEShader::GetResources() const
{
	return std::unordered_map<std::string, H2M::ShaderResourceDeclarationH2M>();
}

void OpenGLImaGEShader::AddShaderReloadedCallback(const ShaderReloadedCallback& callback)
{
}

uint32_t OpenGLImaGEShader::GetRendererID() const
{
	return programID;
}

size_t OpenGLImaGEShader::GetHash() const
{
	return std::hash<std::string>{}(m_ShaderFilepath_Vertex + m_ShaderFilepath_Compute + m_ShaderFilepath_Fragment);
}

void OpenGLImaGEShader::SetUniformBuffer(const std::string& name, const void* data, uint32_t size)
{
}

void OpenGLImaGEShader::SetUniform(const std::string& fullname, float value)
{
	H2M_CORE_ASSERT(m_UniformLocations.find(fullname) != m_UniformLocations.end());
	GLint location = m_UniformLocations.at(fullname);
	glUniform1f(location, value);
}

void OpenGLImaGEShader::SetUniform(const std::string& fullname, uint32_t value)
{
	H2M_CORE_ASSERT(m_UniformLocations.find(fullname) != m_UniformLocations.end());
	GLint location = m_UniformLocations.at(fullname);
	glUniform1ui(location, value);
}

void OpenGLImaGEShader::SetUniform(const std::string& fullname, int value)
{
	H2M_CORE_ASSERT(m_UniformLocations.find(fullname) != m_UniformLocations.end());
	GLint location = m_UniformLocations.at(fullname);
	glUniform1i(location, value);
}

void OpenGLImaGEShader::SetUniform(const std::string& fullname, const glm::vec2& value)
{
	H2M_CORE_ASSERT(m_UniformLocations.find(fullname) != m_UniformLocations.end());
	GLint location = m_UniformLocations.at(fullname);
	glUniform2fv(location, 1, glm::value_ptr(value));
}

void OpenGLImaGEShader::SetUniform(const std::string& fullname, const glm::vec3& value)
{
	H2M_CORE_ASSERT(m_UniformLocations.find(fullname) != m_UniformLocations.end());
	GLint location = m_UniformLocations.at(fullname);
	glUniform3fv(location, 1, glm::value_ptr(value));
}

void OpenGLImaGEShader::SetUniform(const std::string& fullname, const glm::vec4& value)
{
	H2M_CORE_ASSERT(m_UniformLocations.find(fullname) != m_UniformLocations.end());
	GLint location = m_UniformLocations.at(fullname);
	glUniform4fv(location, 1, glm::value_ptr(value));
}

void OpenGLImaGEShader::SetUniform(const std::string& fullname, const glm::mat3& value)
{
	H2M_CORE_ASSERT(m_UniformLocations.find(fullname) != m_UniformLocations.end());
	GLint location = m_UniformLocations.at(fullname);
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void OpenGLImaGEShader::SetUniform(const std::string& fullname, const glm::mat4& value)
{
	H2M_CORE_ASSERT(m_UniformLocations.find(fullname) != m_UniformLocations.end());
	GLint location = m_UniformLocations.at(fullname);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void OpenGLImaGEShader::SetUInt(const std::string& name, uint32_t value)
{
	int32_t location = GetUniformLocation(name);
	glUniform1ui(location, value);
}

void OpenGLImaGEShader::SetInt(const std::string& name, int value)
{
	int32_t location = GetUniformLocation(name);
	glUniform1i(location, value);
}

void OpenGLImaGEShader::SetBool(const std::string& name, bool value)
{
	int32_t location = GetUniformLocation(name);
	glUniform1i(location, value);
}

void OpenGLImaGEShader::SetFloat(const std::string& name, float value)
{
	//	auto uniformLocation = GetUniformLocation(name);
	//	if (uniformLocation != -1) {
	//		glUniform1f(uniformLocation, value);
	//	}
	//	else {
	//		Log::GetLogger()->error("OpenGLImaGEShader::SetFloat() failed [name='{0}', location='{1}']", name, uniformLocation);
	//	}

	int32_t location = GetUniformLocation(name);
	glUniform1f(location, value);
}

void OpenGLImaGEShader::SetFloat2(const std::string& name, const glm::vec2& value)
{
	//	glUseProgram(programID);
	//	auto location = glGetUniformLocation(programID, name.c_str());
	//	if (location != -1) {
	//		glUniform2f(location, value.x, value.y);
	//	}
	//	else {
	//		Log::GetLogger()->error("OpenGLImaGEShader::SetFloat2 - uniform '{0}' not found!", name);
	//	}

	int32_t location = GetUniformLocation(name);
	glUniform2f(location, value.x, value.y);
}

void OpenGLImaGEShader::SetFloat3(const std::string& name, const glm::vec3& value)
{
	//	glUseProgram(programID);
	//	auto location = glGetUniformLocation(programID, name.c_str());
	//	if (location != -1) {
	//		glUniform3f(location, value.x, value.y, value.z);
	//	}
	//	else {
	//		Log::GetLogger()->error("OpenGLImaGEShader::SetFloat3 - uniform '{0}' not found!", name);
	//	}

	int32_t location = GetUniformLocation(name);
	glUniform3f(location, value.x, value.y, value.z);
}

void OpenGLImaGEShader::SetFloat4(const std::string& name, const glm::vec4& value)
{
	//	glUseProgram(programID);
	//	auto location = glGetUniformLocation(programID, name.c_str());
	//	if (location != -1) {
	//		glUniform4f(location, value.x, value.y, value.z, value.w);
	//	}
	//	else {
	//		Log::GetLogger()->error("OpenGLImaGEShader::SetFloat4 - uniform '{0}' not found!", name);
	//	}

	int32_t location = GetUniformLocation(name);
	glUniform4f(location, value.x, value.y, value.z, value.w);
}

void OpenGLImaGEShader::SetMat4(const std::string& name, const glm::mat4& value)
{
	//	glUseProgram(programID);
	//	auto location = glGetUniformLocation(programID, name.c_str());
	//	if (location != -1) {
	//		glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)&value);
	//	}
	//	else {
	//		Log::GetLogger()->error("OpenGLImaGEShader::SetMat4 - uniform '{0}' not found!", name);
	//	}

	int32_t location = GetUniformLocation(name);
	glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)&value);
}

void OpenGLImaGEShader::UploadUniformMat4(const std::string& name, const glm::mat4& values)
{
	//	glUseProgram(programID);
	//	auto location = glGetUniformLocation(programID, name.c_str());
	//	if (location != -1) {
	//		glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)&values);
	//	}
	//	else {
	//		Log::GetLogger()->error("OpenGLImaGEShader::UploadUniformMat4 - uniform '{0}' not found!", name);
	//	}

	int32_t location = GetUniformLocation(name);
	glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)&values);
}

void OpenGLImaGEShader::SetMat4FromRenderThread(const std::string& name, const glm::mat4& value, bool bind)
{
	if (bind)
	{
		UploadUniformMat4(name, value);
	}
	else
	{
		int location = glGetUniformLocation(programID, name.c_str());
		if (location != -1) {
			UploadUniformMat4(location, value);
		}
	}
}

void OpenGLImaGEShader::UploadUniformMat4(uint32_t location, const glm::mat4& value)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void OpenGLImaGEShader::SetLightMatrices(std::vector<glm::mat4> lightMatrices)
{
	for (GLuint i = 0; i < 6; i++)
	{
		glUniformMatrix4fv(uniformLightMatrices[i], 1, GL_FALSE, glm::value_ptr(lightMatrices[i]));
	}
}

void OpenGLImaGEShader::setLightMat4(std::vector<glm::mat4> lightMatrices)
{
	for (GLuint i = 0; i < 6; i++)
	{
		std::string name = "lightMatrices[" + std::to_string(i) + "]";
		SetMat4(name, lightMatrices[i]);
	}
}

void OpenGLImaGEShader::SetVec2(const std::string& name, const glm::vec2& value)
{
	glUniform2fv(GetUniformLocation(name), 1, &value[0]);
}

void OpenGLImaGEShader::SetVec2(const std::string& name, float x, float y)
{
	glUniform2f(GetUniformLocation(name), x, y);
}

void OpenGLImaGEShader::SetMat2(const std::string& name, const glm::mat2& mat)
{
	glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void OpenGLImaGEShader::SetMat3(const std::string& name, const glm::mat3& mat)
{
	glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void OpenGLImaGEShader::Bind()
{
	glUseProgram(programID);
}

void OpenGLImaGEShader::Reload(bool forceCompile)
{
	ClearShader();

	programID = glCreateProgram();

	if (!m_ShaderFilepath_Vertex.empty()) {
		CreateFromFileVertex(m_ShaderFilepath_Vertex.c_str());
	}

	if (!m_ShaderFilepath_Fragment.empty()) {
		CreateFromFileFragment(m_ShaderFilepath_Fragment.c_str());
	}

	if (!m_ShaderFilepath_Geometry.empty()) {
		CreateFromFileGeometry(m_ShaderFilepath_Geometry.c_str());
	}

	if (!m_ShaderFilepath_Compute.empty()) {
		CreateFromFileCompute(m_ShaderFilepath_Compute.c_str());
	}

	CompileProgram();
}

void OpenGLImaGEShader::Unbind()
{
	glUseProgram(0);
}

void OpenGLImaGEShader::ClearShader()
{
	if (programID > 0)
	{
		glDeleteProgram(programID);
		programID = 0;
	}

	m_UniformLocations.clear();
}

OpenGLImaGEShader::~OpenGLImaGEShader()
{
	ClearShader();
}

void OpenGLImaGEShader::CompileShader(const char* vertexCode, const char* fragmentCode)
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

void OpenGLImaGEShader::CompileShader(const char* vertexCode, const char* geometryCode, const char* fragmentCode)
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

void OpenGLImaGEShader::AddShaderVertex(const char* vertexCode)
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

void OpenGLImaGEShader::AddShaderFragment(const char* fragmentCode)
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

void OpenGLImaGEShader::AddShaderGeometry(const char* geometryCode)
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

void OpenGLImaGEShader::AddShaderCompute(const char* computeCode)
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

const char* OpenGLImaGEShader::GetShaderTypeNameFromEnum(const GLenum shaderType)
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

void OpenGLImaGEShader::AddShader(GLuint programID, const char* shaderCode, GLenum shaderType)
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

void OpenGLImaGEShader::CompileProgram()
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

void OpenGLImaGEShader::GetUniformLocations()
{
	for (unsigned int i = 0; i < 6; i++)
	{
		char locBuff[100] = { '\0' };

		snprintf(locBuff, sizeof(locBuff), "lightMatrices[%d]", i);
		uniformLightMatrices[i] = glGetUniformLocation(programID, locBuff);
	}
}
