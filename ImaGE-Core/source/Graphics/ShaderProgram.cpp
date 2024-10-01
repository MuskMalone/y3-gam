#include <pch.h>
#include <fstream>
#include <sstream>
#include "ShaderProgram.h"

namespace
{
  const char* getTypeString(GLenum type)
  {
    // There are many more types than are covered here, but
    // these are the most common in these examples.
    switch (type) {
    case GL_FLOAT:
      return "float";
    case GL_FLOAT_VEC2:
      return "vec2";
    case GL_FLOAT_VEC3:
      return "vec3";
    case GL_FLOAT_VEC4:
      return "vec4";
    case GL_DOUBLE:
      return "double";
    case GL_INT:
      return "int";
    case GL_UNSIGNED_INT:
      return "unsigned int";
    case GL_BOOL:
      return "bool";
    case GL_FLOAT_MAT2:
      return "mat2";
    case GL_FLOAT_MAT3:
      return "mat3";
    case GL_FLOAT_MAT4:
      return "mat4";
    default:
      return "?";
    }
  }
}

namespace Graphics
{

  ShaderProgram::ShaderProgram() : m_handle{ glCreateProgram() }, m_linked{ false } {}

  ShaderProgram::ShaderProgram(GLuint handle)
    : m_handle(handle), m_linked(false)
  {
  }

  ShaderProgram::ShaderProgram(const char* sv, const char* sf)
    : m_handle(0), m_linked(false)
  {
    if (sv && sf && strlen(sv) && strlen(sf))
    {
      CompileShader(sv, GL_VERTEX_SHADER);
      CompileShader(sf, GL_FRAGMENT_SHADER);
      Link();
    }
  }

  ShaderProgram::~ShaderProgram()
  {
    if (!m_handle || !m_linked)
      return;
    DetachAndDeleteShaderObjects();
    // Delete the program
    glDeleteProgram(m_handle);
  }

  void ShaderProgram::DetachAndDeleteShaderObjects()
  {
    // Detach and delete the shader objects (if they are not already removed)
    GLint numShaders = 0;
    glGetProgramiv(m_handle, GL_ATTACHED_SHADERS, &numShaders);
    std::vector<GLuint> shaderNames(numShaders);
    glGetAttachedShaders(m_handle, numShaders, NULL, shaderNames.data());
    for (GLuint shader : shaderNames)
    {
      glDetachShader(m_handle, shader);
      glDeleteShader(shader);
    }
  }

  void ShaderProgram::CompileShaderFile(const char* vtxShaderFile, const char* fragShaderFile)
  {
    std::ifstream ifsV{ vtxShaderFile, std::ios::in }, ifsF{ fragShaderFile, std::ios::in };
    if (!ifsV) {
      throw std::runtime_error(std::string("Unable to open: ") + vtxShaderFile);
    }
    else if (!ifsF) {
      throw std::runtime_error(std::string("Unable to open: ") + fragShaderFile);
    }

    // Get file contents
    std::stringstream vtxCode, fragCode;
    vtxCode << ifsV.rdbuf();
    fragCode << ifsF.rdbuf();

    ifsV.close();
    ifsF.close();

    CompileShader(vtxCode.str().c_str(), GL_VERTEX_SHADER);
    CompileShader(fragCode.str().c_str(), GL_FRAGMENT_SHADER);
    Link();
  }

  void ShaderProgram::CompileShader(const char* code, GLenum type)
  {
    if (!m_handle)
    {
      m_handle = glCreateProgram();
      if (!m_handle)
        throw std::runtime_error("Unable to create shader program.");
    }

    GLuint shaderHandle = glCreateShader(type);

    glShaderSource(shaderHandle, 1, &code, NULL);

    // Compile the shader
    glCompileShader(shaderHandle);

    // Check for errors
    int result;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &result);
    if (GL_FALSE == result)
    {
      // Compile failed, get log
      std::string msg = "Shader compilation failed.\n";

      int length = 0;
      glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &length);
      if (length > 0)
      {
        std::string log(length, ' ');
        int written = 0;
        glGetShaderInfoLog(shaderHandle, length, &written, &log[0]);
        msg += log;
      }
      throw std::runtime_error(msg);
    }
    else
    {
      // Compile succeeded, attach shader
      glAttachShader(m_handle, shaderHandle);
    }
  }

  void ShaderProgram::Link()
  {
    if (!m_handle)
      throw std::runtime_error("Program has not been compiled.");

    glLinkProgram(m_handle);
    int status = 0;
    std::string errString;
    glGetProgramiv(m_handle, GL_LINK_STATUS, &status);
    if (GL_FALSE == status)
    {
      // Store log and return false
      int length = 0;
      glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &length);
      errString += "Program link failed:\n";
      if (length > 0)
      {
        std::string log(length, ' ');
        int written = 0;
        glGetProgramInfoLog(m_handle, length, &written, &log[0]);
        errString += log;
      }
    }
    else
    {
      FindUniformLocations();
      m_linked = true;
    }

    DetachAndDeleteShaderObjects();

    if (GL_FALSE == status)
      throw std::runtime_error(errString);
  }

  void ShaderProgram::FindUniformLocations()
  {
    m_uniformLocations.clear();

    GLint numUniforms = 0;

    // For OpenGL 4.3 and above, use glGetProgramResource
    glGetProgramInterfaceiv(m_handle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);

    GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };

    for (GLint i = 0; i < numUniforms; ++i)
    {
      GLint results[4];
      glGetProgramResourceiv(m_handle, GL_UNIFORM, i, 4, properties, 4, NULL, results);

      if (results[3] != -1) continue;  // Skip uniforms in blocks
      GLint nameBufSize = results[0] + 1;
      char* name = new char[nameBufSize];
      glGetProgramResourceName(m_handle, GL_UNIFORM, i, nameBufSize, NULL, name);
      m_uniformLocations[name] = results[2];
      delete[] name;
    }
  }

  void ShaderProgram::Use() const
  {
    glUseProgram(m_handle);
  }

  void ShaderProgram::Unuse() const
  {
    glUseProgram(0);
  }

  void ShaderProgram::PrintActiveUniforms()
  {
    // For OpenGL 4.3 and above, use glGetProgramResource
    GLint numUniforms = 0;
    glGetProgramInterfaceiv(m_handle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);

    GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };

    printf("Active uniforms:\n");
    for (int i = 0; i < numUniforms; ++i) {
      GLint results[4];
      glGetProgramResourceiv(m_handle, GL_UNIFORM, i, 4, properties, 4, NULL, results);

      if (results[3] != -1) continue;  // Skip uniforms in blocks
      GLint nameBufSize = results[0] + 1;
      char* name = new char[nameBufSize];
      glGetProgramResourceName(m_handle, GL_UNIFORM, i, nameBufSize, NULL, name);
      printf("%-5d %s (%s)\n", results[2], name, getTypeString(results[1]));
      delete[] name;
    }
  }

  void ShaderProgram::PrintActiveUniformBlocks()
  {
    GLint numBlocks = 0;

    glGetProgramInterfaceiv(m_handle, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numBlocks);
    GLenum blockProps[] = { GL_NUM_ACTIVE_VARIABLES, GL_NAME_LENGTH };
    GLenum blockIndex[] = { GL_ACTIVE_VARIABLES };
    GLenum props[] = { GL_NAME_LENGTH, GL_TYPE, GL_BLOCK_INDEX };

    for (int block = 0; block < numBlocks; ++block) {
      GLint blockInfo[2];
      glGetProgramResourceiv(m_handle, GL_UNIFORM_BLOCK, block, 2, blockProps, 2, NULL, blockInfo);
      GLint numUnis = blockInfo[0];

      char* blockName = new char[blockInfo[1] + 1];
      glGetProgramResourceName(m_handle, GL_UNIFORM_BLOCK, block, blockInfo[1] + 1, NULL, blockName);
      printf("Uniform block \"%s\":\n", blockName);
      delete[] blockName;

      GLint* unifIndexes = new GLint[numUnis];
      glGetProgramResourceiv(m_handle, GL_UNIFORM_BLOCK, block, 1, blockIndex, numUnis, NULL, unifIndexes);

      for (int unif = 0; unif < numUnis; ++unif) {
        GLint uniIndex = unifIndexes[unif];
        GLint results[3];
        glGetProgramResourceiv(m_handle, GL_UNIFORM, uniIndex, 3, props, 3, NULL, results);

        GLint nameBufSize = results[0] + 1;
        char* name = new char[nameBufSize];
        glGetProgramResourceName(m_handle, GL_UNIFORM, uniIndex, nameBufSize, NULL, name);
        printf("    %s (%s)\n", name, getTypeString(results[1]));
        delete[] name;
      }

      delete[] unifIndexes;
    }
  }

  void ShaderProgram::PrintActiveAttribs()
  {
    // >= OpenGL 4.3, use glGetProgramResource
    GLint numAttribs;
    glGetProgramInterfaceiv(m_handle, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttribs);

    GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };

    printf("Active attributes:\n");
    for (int i = 0; i < numAttribs; ++i) {
      GLint results[3];
      glGetProgramResourceiv(m_handle, GL_PROGRAM_INPUT, i, 3, properties, 3, NULL, results);

      GLint nameBufSize = results[0] + 1;
      char* name = new char[nameBufSize];
      glGetProgramResourceName(m_handle, GL_PROGRAM_INPUT, i, nameBufSize, NULL, name);
      printf("%-5d %s (%s)\n", results[2], name, getTypeString(results[1]));
      delete[] name;
    }
  }

  void ShaderProgram::Validate()
  {
    if (m_linked)
      throw std::runtime_error("Program is not linked");

    GLint status;
    glValidateProgram(m_handle);
    glGetProgramiv(m_handle, GL_VALIDATE_STATUS, &status);

    if (GL_FALSE == status) {
      // Store log and return false
      int length = 0;
      std::string logString;

      glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &length);

      if (length > 0) {
        char* c_log = new char[length];
        int written = 0;
        glGetProgramInfoLog(m_handle, length, &written, c_log);
        logString = c_log;
        delete[] c_log;
      }

      throw std::runtime_error(std::string("Program failed to validate\n") + logString);

    }
  }

  GLint ShaderProgram::GetUniformLocation(const std::string& name)
  {
    auto pos = m_uniformLocations.find(name);
    if (pos == m_uniformLocations.end())
      return m_uniformLocations[name] = glGetUniformLocation(m_handle, name.c_str());
    return pos->second;
  }

} // namespace Graphics
