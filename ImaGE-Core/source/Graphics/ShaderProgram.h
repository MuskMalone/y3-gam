#pragma once

#include <string>
#include <map>
#include <vector>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glad/glad.h>

namespace Graphics
{

  class ShaderProgram
  {
  public:
    GLuint m_handle;

  private:
    bool m_linked;
    std::map<std::string, int> m_uniformLocations;

    GLint GetUniformLocation(const std::string& name);
    void DetachAndDeleteShaderObjects();
    void Link();

  public:
    ShaderProgram();
    ShaderProgram(GLuint handle);
    ShaderProgram(const char* sv, const char* sf);
    ShaderProgram(const ShaderProgram& rhs) = delete;
    ShaderProgram& operator=(const ShaderProgram& rhs) = delete;
    ~ShaderProgram();

    operator GLuint() { return m_handle; }

    void CompileShaderFile(const char* vtxShaderFile, const char* fragShaderFile);
    void CompileShader(const char* code, GLenum type);

    void Validate();
    void Use() const;
    void Unuse() const;

    void BindAttribLocation(GLuint location, const char* name) {
      glBindAttribLocation(m_handle, location, name);
    }

    void BindFragDataLocation(GLuint location, const char* name) {
      glBindFragDataLocation(m_handle, location, name);
    }

    void SetUniform(const std::string& name, float x, float y, float z) {
      glUniform3f(GetUniformLocation(name), x, y, z);
    }

    void SetUniform(const std::string& name, const glm::vec2& v) {
      glUniform2f(GetUniformLocation(name), v.x, v.y);
    }

    void SetUniform(const std::string& name, const glm::vec3& v) {
      SetUniform(name, v.x, v.y, v.z);
    }

    void SetUniform(const std::string& name, const glm::vec4& v) {
      glUniform4f(GetUniformLocation(name), v.x, v.y, v.z, v.w);
    }

    void SetUniform(const std::string& name, const std::vector<float>& v) {
      if (v.size() == 1)
        glUniform1f(GetUniformLocation(name), v[0]);
      else if (v.size() == 2)
        glUniform2f(GetUniformLocation(name), v[0], v[1]);
      else if (v.size() == 3)
        glUniform3f(GetUniformLocation(name), v[0], v[1], v[2]);
      else if (v.size() == 4)
        glUniform4f(GetUniformLocation(name), v[0], v[1], v[2], v[3]);
    }

    void SetUniform(const std::string& name, const glm::mat4& m) {
      glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &m[0][0]);
    }

    void SetUniform(const std::string& name, const glm::mat3& m) {
      glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &m[0][0]);
    }

    void SetUniform(const std::string& name, float val) {
      glUniform1f(GetUniformLocation(name), val);
    }

    void SetUniform(const std::string& name, int val) {
      glUniform1i(GetUniformLocation(name), val);
    }

    void SetUniform(const std::string& name, bool val) {
      glUniform1i(GetUniformLocation(name), val);
    }

    void SetUniform(const std::string& name, GLuint val) {
      glUniform1ui(GetUniformLocation(name), val);
    }

    void FindUniformLocations();

    void PrintActiveUniforms();
    void PrintActiveUniformBlocks();
    void PrintActiveAttribs();
  };

} // namespace Graphics